#pragma once

#include "Agent.h"
#include "SwingPointVector.h"
#include "MATrendClassifier.h"

class SPSupportAgent : public Agent {
	SwingPointVector spvec ;
	MATrendClassifier matc ;	// used to classify trend
	const CandlestickSeries *pcss ;
	const double lthresh ;
	const unsigned lookback ;

	double proximity(double price, double level) ;

public:
	SPSupportAgent(const CandlestickSeries *_psrc, double spthresh, double _lthresh, unsigned _lookback) ; 
	void update() ;
	std::wstring toString() { return L"SPSupportAgent" ; }
} ;