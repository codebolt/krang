#include "CandlestickSeries.h"
#include "Candlestick.h"
#include "Trade.h"
#include "Agent.h"
#include "Simulation.h"

#include "boost/date_time/posix_time/posix_time.hpp"

CandlestickSeries::CandlestickSeries(boost::posix_time::time_period initial) 
	: id(idgen++), current(initial), duration(initial.length()) {
	// TODO: reserve the series vector (need to estimate tight upper bound on how many candles are in the series)
	series.reserve(256);
}

void CandlestickSeries::nextTrade() {
	using namespace boost::posix_time ;
	using namespace boost::gregorian ;

	const Candlestick& lastACS = Simulation::getSingleton()->lastAtomic() ;	// last atomic candlestick
	ptime now = lastACS.tp.last() ;

	if(series.empty()) {
		// insert first candle
		current = time_period( ptime(now.date(), hours(0)), duration ) ; 
		while(!(current.contains(now))) current.shift(duration) ;
		assert(current.contains(lastACS.tp));
		Candlestick cs = { 0, lastACS.volume, lastACS.close, lastACS.open, lastACS.high, lastACS.low, current } ;
		addCandle(cs) ;
	}
	else if(current.contains(now)) {
		// update current candle
		assert(current.contains(lastACS.tp)) ;
		Candlestick& last = series.back() ;
		last.close = lastACS.close ;
		last.volume += lastACS.volume ;
		if(lastACS.high>last.high) last.high = lastACS.high;
		else if(lastACS.low<last.low) last.low = lastACS.low;
		updateAgents(now) ;
	}
	else if(now.date() == current.begin().date()) {
		// insert new candle(s) in the same day
		// the new is on the same date, so we create blank candles for the in-betweens
		current.shift(duration) ;
		assert(current.begin() == lastACS.tp.begin()) ;	// has to be true for any of this to make sense
		Candlestick cs = { series.back().cnt+1, lastACS.volume, lastACS.close, lastACS.open, lastACS.high, lastACS.low, current } ;
		addCandle(cs);
		updateAgents(now) ;
	}
	else {
		// not the same day, skip to the trade (no blank in-betweens)
		current = time_period( ptime(now.date(), hours(0)), duration ) ; 
		while(!current.contains(now)) current.shift(duration) ;
		assert(current.contains(lastACS.tp)) ;
		Candlestick cs = { series.back().cnt+1, lastACS.volume, lastACS.close, lastACS.open, lastACS.high, lastACS.low, current } ;
		addCandle(cs) ;
		updateAgents(now) ;
	}
}

std::wstring CandlestickSeries::toString() const {
	return boost::posix_time::to_simple_wstring( duration ) ;
}

void CandlestickSeries::updateAgents(const boost::posix_time::ptime& pt) {
	for( std::vector<Agent*>::iterator it=agents.begin();it!=agents.end();it++ ) (*it)->update(pt) ;
}


unsigned CandlestickSeries::idgen = 0 ;
