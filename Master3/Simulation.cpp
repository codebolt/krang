#include "Simulation.h"
#include "MainDlg.h"
#include "CandlestickSeries.h"
#include "Agent.h"
#include "NeuralNetwork.h"
#include "AnnWindow.h"

Simulation Simulation::singleton ;

void Simulation::step() {
	if(state == STATE_RUNNING) {
		// todo
		if(tradeIter == ts->end()) {
			setState(STATE_FINISHED) ;
			ann->finalize() ;
			AnnWindow::getSingleton()->displayLogWnd() ;
			MainDlg::log(L"Simulation finished.\r\n");
		}
		else {
			atomicStep() ;
			for(CssIt css = cssvec.begin(); css != cssvec.end(); css++) {
				(*css)->nextTrade() ;
			}
			ann->update() ;
		}
	}
	else if(state == STATE_INIT) {
		// todo
		MainDlg::getSingleton()->updateStatus() ;
		MainDlg::log(L"Initializing...\r\n");
		doInit() ;
		setState(STATE_RUNNING) ;
		MainDlg::log(L"Finished initialization.\r\nRunning simulation...\r\n");
	}
}

void Simulation::setState(Simulation::State _state) {
	// only certain transitions are allowed:
	// RUNNING -> PAUSED
	// PAUSED -> RUNNING
	// INIT -> RUNNING
	// FINISHED -> RUNNING
	if( (_state == STATE_PAUSED && state == STATE_RUNNING) ||
		(_state == STATE_RUNNING && (state == STATE_PAUSED || state == STATE_INIT)) ||
		(_state == STATE_FINISHED && state == STATE_RUNNING) ) 
	{
		if(_state == STATE_PAUSED) MainDlg::log(L"Simulation paused.\r\n") ;
		if(state == STATE_PAUSED) MainDlg::log(L"Simulation resumed.\r\n") ;
		state = _state ;
	}

	MainDlg::getSingleton()->updateStatus() ;
}

double Simulation::progress() const {
	if((state == STATE_RUNNING) || (state == STATE_PAUSED)) {
		double tcnt = tradeIter->cnt ;
		double tsz = ts->size() ;
		return 100.0 * tcnt / tsz ;
	}
	else if(state == STATE_FINISHED) {
		return 100.0 ;
	}
	else {
		return 0.0 ;
	}
}

std::wstring Simulation::status() const {
	switch(state) {
	case STATE_INIT: return L"Initializing" ;
	case STATE_RUNNING: return L"Running" ;
	case STATE_PAUSED: return L"Paused" ;
	case STATE_FINISHED: return L"Finished" ;
	default: return L"err" ;
	}
}

Simulation::~Simulation() {
	for(CssIt css = cssvec.begin(); css != cssvec.end(); css++) {
		delete (*css) ;
	}
	for(AgentIt agent = agentvec.begin(); agent != agentvec.end(); agent++) {
		delete (*agent) ;
	}
}

void Simulation::atomicStep() {
	using namespace boost::posix_time;
	static time_duration dur = time_duration(0,1,0,0) ;
	const Trade& t = *tradeIter ;

	if(atomicCSS.empty()) {
		time_period tp( ptime(t.time.date(),hours(0)) , dur );	// 1 minute resolution
		while(!tp.contains(t.time)) tp.shift( dur ) ;
		Candlestick firstAtomic = { 0, t.volume, t.price, t.price, t.price, t.price, tp } ;
		atomicCSS.push_back(firstAtomic) ;
		++ tradeIter ;
	}
	else {
		const Candlestick& prev = atomicCSS.back() ;

		assert( prev.tp.is_before(t.time) ) ;

		if(prev.tp.begin().date() == t.time.date()) {
			time_period tp(prev.tp) ;
			tp.shift( dur ) ;
			Candlestick nextAtomic = { prev.cnt + 1, 0, prev.close, prev.close, prev.close, prev.close, tp } ;
			atomicCSS.push_back(nextAtomic) ;
			// don't inc tradeIter (not contained in this candle)
		}
		else {
			time_period tp( ptime(t.time.date(),hours(0)), dur ) ;
			while(tp.is_before( t.time )) tp.shift( dur ) ;
			Candlestick nextAtomic = { prev.cnt + 1, t.volume, t.price, t.price, t.price, t.price, tp } ;
			assert(nextAtomic.tp.contains(t.time)) ;
			atomicCSS.push_back(nextAtomic) ;
			++ tradeIter ;
		}
	}

	Candlestick& last = atomicCSS.back() ;

	while((tradeIter!=ts->end()) && last.tp.contains(tradeIter->time)) {
		if(last.volume==0) {
			last.close = last.open = last.high = last.low = tradeIter->price ;
			last.volume = tradeIter->volume ;
		}
		else {
			last.close = tradeIter->price ;
			if(tradeIter->price > last.high) last.high = tradeIter->price ;
			if(tradeIter->price < last.low) last.low = tradeIter->price ;
			last.volume += tradeIter->volume ;
		}

		++ tradeIter ;
	}

	if(tradeIter!=ts->end()) {
		assert(last.tp.is_before( tradeIter->time )) ;
	}
}