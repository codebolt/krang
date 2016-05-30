#pragma warning( disable : 244 )	// annoying warning about converting wchar to char

#include "Simulation.h"

#include "CandlestickSeries.h"
#include "MATrendAgent.h"
#include "MASupportAgent.h"
#include "VolumeAgent.h"
#include "RsiLevelAgent.h"
#include "RsiDivAgent.h"
#include "SPSupportAgent.h"
#include "FibonacciAgent.h"
#include "DoubleTopAgent.h"
#include "DoubleBottomAgent.h"
#include "TrendLineAgent.h"

#include "NeuralNetwork.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>


#include <cstring>

#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "boost/filesystem/operations.hpp"

// initialization is a pain in the ass

void Simulation::doInit() {
//	using namespace boost::gregorian ;
//	using namespace boost::posix_time ;

	using namespace xercesc ;

	try {
		XercesDOMParser *parser = new XercesDOMParser() ;

		parser->setValidationScheme( XercesDOMParser::Val_Never ) ;
		parser->setDoNamespaces( false ); 

		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        parser->setErrorHandler(errHandler);

		// check that file exists
		SIM_ASSERT( boost::filesystem::exists( xmlfile ) );

		// parse the simulation xml file
		parser->parse( this->xmlfile.c_str() ) ;

		DOMDocument *doc = parser->getDocument() ;
		DOMNodeList *nodes = doc->getDocumentElement()->getChildNodes() ;

		bool dataInit = false, annInit = false ;

		for(unsigned i=0; i<nodes->getLength(); i++) {
			if(nodes->item(i)->getNodeType() == DOMNode::ELEMENT_NODE) {
				DOMElement *elem = (DOMElement*) nodes->item(i) ;
				if(!_wcsicmp( elem->getNodeName(), L"data" )) {
					assert(!dataInit) ;
					// initialize the data
					this->initData( elem->getAttribute(L"ticker"), elem->getAttribute(L"from"), elem->getAttribute(L"to") ) ;
					dataInit = true ;
				}
				else if(!_wcsicmp( elem->getNodeName(), L"ann" )) {
					assert(!annInit) ;
					// initialize the network
					bool train = !_wcsicmp( doc->getDocumentElement()->getAttribute(L"type"), L"train" ) ;
					DOMNodeList* trainCfgElems = doc->getElementsByTagName(L"traincfg");
					SIM_ASSERT( !!trainCfgElems->getLength() ) ;
					DOMElement* tcfgElem = (DOMElement*) trainCfgElems->item(0) ;

					bool load = !_wcsicmp( tcfgElem->getAttribute(L"load"), L"true" ) ;
					double lrate = _wtof( tcfgElem->getAttribute(L"lrate") ) ;
					double lmom = _wtof( tcfgElem->getAttribute(L"lmom")  ) ;

					this->initAnn( elem->getAttribute(L"name"), train, load, lrate, lmom ) ;
					annInit = true ;
				}
			}
		}
		assert(dataInit && annInit) ;


		delete parser ;
	}
	catch (const XMLException& toCatch) {
		std::wstring message = toCatch.getMessage();
		assert(0) ;	
	}
	catch (const DOMException& toCatch) {
		std::wstring message = toCatch.getMessage();
		assert(0) ;
	}

	
//	date_period dp( date(2002,Jan,1), date(2008,Jan,1) );
//
//	ts = new TradeSeries() ;
//	ts->read(NAS,dp,L"D:\\tradedata\\");
//	ts->filter();
//
//	tradeIter = ts->begin() ;
//

}

void Simulation::initData(const std::wstring &ticker, const std::wstring &from, const std::wstring &to)
{
	using namespace boost::gregorian ;

	this->ts = new TradeSeries() ;

	// convert date strings to ascii
	std::string afrom(from.length(),' '), ato(to.length(), ' ') ;
	std::copy(from.begin(), from.end(), afrom.begin());
	std::copy(to.begin(), to.end(), ato.begin());

	date date0 = from_string(afrom) ;
	date date1 = from_string(ato) ;

	ts->read( tickerFromString(ticker), date_period(date0, date1), L"D:\\tradedata\\" );
	ts->filter() ;

	tradeIter = ts->begin() ;
}

void Simulation::initAnn(const std::wstring &name, bool train, bool load, double lrate, double lmom)
{
	// this one is called from doInit, so we can let any exceptions fall through here
	using namespace xercesc ;
	using namespace boost::posix_time ;

	XercesDOMParser *parser = new XercesDOMParser() ;

	std::wstring xmlfile = name + L".xml" ;

	parser->parse(xmlfile.c_str()) ;

	DOMDocument *doc = parser->getDocument() ;

	SIM_ASSERT( !_wcsicmp( doc->getDocumentElement()->getAttribute(L"name"), name.c_str()) ) ;

	// set up the inputs

	DOMNodeList* agentElems = doc->getElementsByTagName(L"agent") ;

	SIM_ASSERT( !!agentElems->getLength() ) ;

	for(unsigned i=0; i<agentElems->getLength(); i++) {
		DOMElement* agElem = (DOMElement*) agentElems->item(i) ;

		std::wstring type = agElem->getAttribute(L"class") ;
		std::wstring duration = agElem->getAttribute(L"duration") ;

		CandlestickSeries *pcss = getSeries(duration) ;

		Agent *pAgent ;

		if( !type.compare(L"MATrendAgent") ) {
			// create a moving average agent
			long lint = _wtol( agElem->getAttribute(L"longint") ) ;
			long sint = _wtol( agElem->getAttribute(L"shortint") ) ;
			SIM_ASSERT( (lint>0) && (sint>0) ) ;
			pAgent = new MATrendAgent(pcss, lint, sint) ;

		}
		else if( !type.compare(L"MASupportAgent") ) {
			long intervals = _wtol( agElem->getAttribute(L"intervals") ) ;
			double threshold = _wtof( agElem->getAttribute(L"threshold") ) ;
			SIM_ASSERT( (threshold>0.0) && (intervals>0) ) ;
			pAgent = new MASupportAgent(pcss, intervals, threshold) ;
		}
		else if( !type.compare(L"VolumeAgent") ) {
			// create a volume agent
			long lint = _wtol( agElem->getAttribute(L"lintervals") ) ;
			long sint = _wtol( agElem->getAttribute(L"sintervals") ) ;
			SIM_ASSERT( (lint > sint) && (sint > 0) ) ;
			pAgent = new VolumeAgent(pcss, lint, sint) ;
		}
		else if( !type.compare(L"RSILevelAgent") ) {
			long intervals = _wtol( agElem->getAttribute(L"intervals") ) ;
			SIM_ASSERT(intervals > 0) ;
			pAgent = new RsiLevelAgent(pcss, intervals) ;
		}
		else if( !type.compare(L"RSIDivAgent") ) {
			long intervals = _wtol( agElem->getAttribute(L"intervals") ) ;
			double csthresh = _wtof( agElem->getAttribute(L"csthresh") ) ;
			SIM_ASSERT( (intervals>0) && (csthresh>0.0) ) ;
			pAgent = new RsiDivAgent(pcss, intervals, csthresh) ;
		}
		else if( !type.compare(L"SPSupportAgent") ) {
			long lookback = _wtol( agElem->getAttribute(L"lookback") ) ;
			double spthresh = _wtof( agElem->getAttribute(L"spthresh") ) ;
			double pthresh = _wtof( agElem->getAttribute(L"pthresh") ) ;
			SIM_ASSERT( (lookback>0) && (spthresh>0.0) && (pthresh>0.0) ) ;
			pAgent = new SPSupportAgent(pcss, spthresh, pthresh, lookback) ;
		}
		else if( !type.compare(L"FibonacciAgent") ) {
			double spthresh = _wtof( agElem->getAttribute(L"spthresh") ) ;
			double pthresh = _wtof( agElem->getAttribute(L"pthresh") ) ;
			SIM_ASSERT( (spthresh>0.0) && (pthresh>0.0) ) ;
			pAgent = new FibonacciAgent(pcss, spthresh, pthresh) ;
		}
		else if( !type.compare(L"DoubleTopAgent") ) {
			double spthresh = _wtof( agElem->getAttribute(L"spthresh") ) ;
			double pthresh = _wtof( agElem->getAttribute(L"pthresh") ) ;
			SIM_ASSERT( (spthresh>0.0) && (pthresh>0.0) ) ;
			pAgent = new DoubleTopAgent(pcss, spthresh, pthresh) ;
		}
		else if( !type.compare(L"DoubleBottomAgent") ) {
			double spthresh = _wtof( agElem->getAttribute(L"spthresh") ) ;
			double pthresh = _wtof( agElem->getAttribute(L"pthresh") ) ;
			SIM_ASSERT( (spthresh>0.0) && (pthresh>0.0) ) ;
			pAgent = new DoubleBottomAgent(pcss, spthresh, pthresh) ;
		}
		else if( !type.compare(L"TrendLineAgent") ) {
			double spthresh = _wtof( agElem->getAttribute(L"spthresh") ) ;
			double pthresh = _wtof( agElem->getAttribute(L"pthresh") ) ;
			SIM_ASSERT( (spthresh>0.0) && (pthresh>0.0) ) ;
			pAgent = new TrendLineAgent(pcss, spthresh, pthresh) ;
		}
		else {
			throw SIM_EX(L"Illegal agent type: " + type) ;
		}

		agentvec.push_back( pAgent ) ;
		pcss->addAgent( pAgent ) ;
	}

	// get the hidden layer sizes
	std::vector<int> hiddenLayers ;
	DOMNodeList* hiddenElems = doc->getElementsByTagName(L"hlayer");

	for(unsigned i=0; i<hiddenElems->getLength(); i++) {
		DOMElement* layerElem = (DOMElement*) hiddenElems->item(i) ;

		std::wstring sizeW = layerElem->getAttribute(L"size") ;
		int size = _wtol( sizeW.c_str() ) ;
		SIM_ASSERT(size > 0) ;
		hiddenLayers.push_back(size) ;
	}

	// set up the outputs
	DOMNodeList* outElems = doc->getElementsByTagName(L"out") ;

	assert( !!outElems->getLength() );

	std::vector<time_duration> durationVec ;

	durationVec.reserve(outElems->getLength()) ;

	for(unsigned i=0; i<outElems->getLength(); i++) {
		DOMElement* outElem = (DOMElement*) outElems->item(i) ;

		std::wstring durationW = outElem->getAttribute(L"duration") ;
		std::string durationA( durationW.length(), ' ' ) ;
		std::copy( durationW.begin(), durationW.end(), durationA.begin() ) ;

		durationVec.push_back( duration_from_string(durationA) ) ;

		SIM_ASSERT(!durationVec.back().is_not_a_date_time()) ;
	}

	this->ann = new NeuralNetwork(agentvec, ts, durationVec, name, hiddenLayers, train) ;

	// init for train or test

	if(train) {
		if(load) ann->load() ;
		else ann->create() ;

		ann->configtrain(lrate, lmom) ;
	}
	else ann->load() ;			// load the network (already been trained)
}

CandlestickSeries* Simulation::getSeries(const std::wstring &durationStr) {
	using namespace boost::posix_time ;

	std::string durationAStr(durationStr.length(), ' ') ;
	std::copy( durationStr.begin(), durationStr.end(), durationAStr.begin() ) ;

	time_duration duration = duration_from_string(durationAStr);

	for( CssIt it=cssvec.begin(); it != cssvec.end(); it++ ) {
		if( (*it)->getDuration() == duration ) return *it ;
	}

	// need to create a new
	boost::gregorian::date begin = ts->begin()->time.date() ;
	CandlestickSeries* css = new CandlestickSeries( time_period(ptime(begin,hours(0)), duration) ) ;
	cssvec.push_back(css) ;

	return css ;
}