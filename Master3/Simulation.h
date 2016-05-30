#pragma once

#include "TradeSeries.h"
#include "Candlestick.h"

class CandlestickSeries ;
class CalendarIterator ;
class Agent ;
class NeuralNetwork ;

class Simulation {
	static Simulation singleton ;

	TradeSeries* ts ;
	TradeSeries::ConstIterator tradeIter ;

	std::vector<Candlestick> atomicCSS ;	// atomic candle series (1-minute)

	NeuralNetwork *ann ;

	std::vector<CandlestickSeries*> cssvec ;
	std::vector<CalendarIterator*> tipvec ;
	std::vector<Agent*> agentvec ;

	std::wstring xmlfile ;	// path to the simulation xml file

	typedef std::vector<CandlestickSeries*>::iterator CssIt ;
	typedef std::vector<Agent*>::iterator AgentIt ;

	enum State {
		STATE_INIT,
		STATE_RUNNING,
		STATE_PAUSED,
		STATE_FINISHED
	} state ;

	CandlestickSeries *getSeries(const std::wstring& duration) ;
	void initData(const std::wstring& ticker, const std::wstring& from, const std::wstring& to) ;
	void initAnn(const std::wstring& name, bool train, bool load, double lrate, double lmom) ;

	void atomicStep() ;	// add the next atomic candle

	void setState(State state);

	Simulation() : state(STATE_INIT),ts(0),ann(0) { }
	~Simulation();

public:
	static Simulation* getSingleton() { return &singleton; }

	const std::vector< CandlestickSeries* >& getSeries() const { return cssvec; }
	NeuralNetwork* getAnn() { return ann ; }

	const Candlestick& lastAtomic() const { return atomicCSS.back(); }

	bool running() const { return (state==STATE_INIT) || (state==STATE_RUNNING) ; }
	bool initialized() const { return state!=STATE_INIT ; }

	void step() ;

	void setXmlFile(const std::wstring& _xmlfile) { xmlfile = _xmlfile; }

	void pause() { setState(STATE_PAUSED); }
	void resume() { setState(STATE_RUNNING); }

	void doInit() ;	// run initialization

	double progress() const ;	// current progress
	std::wstring status() const ;		// current status
};