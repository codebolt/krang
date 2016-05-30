#pragma once

#include "Agent.h"
#include "SwingPointVector.h"

class TrendLineAgent : public Agent {
	const CandlestickSeries *psrc ;
	SwingPointVector spv ;
	double pthresh ;

	double proximity(double price, double target) ;

	double test(const SyncValue& sv1, const SyncValue& sv2, bool sup) ;

public:
	TrendLineAgent(const CandlestickSeries *_psrc, double spthresh, double _pthresh) : Agent(), psrc(_psrc), spv(_psrc, spthresh), pthresh(_pthresh) { }

	void update() ;
	std::wstring toString() { return L"TrendLineAgent"; }
} ;