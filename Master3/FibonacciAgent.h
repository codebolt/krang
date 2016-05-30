#pragma once

#include "Agent.h"
#include "SwingPointVector.h"
#include "MATrendClassifier.h"

class FibonacciAgent : public Agent {
	const CandlestickSeries *psrc ;
	SwingPointVector spv ;
	MATrendClassifier matc ;

	SyncValue prev ;

	double pthresh ;

	double proximity(double price, double level) ;
	double fib(double price, double pivot, double diff) ;

public:
	FibonacciAgent(const CandlestickSeries *_psrc, double spthresh, double _pthresh)
		: Agent(), psrc(_psrc), spv(_psrc, spthresh), pthresh(_pthresh), matc(_psrc, 14, 7, 3)
	{ 
		transforms.push_back(&spv);
		prev.cscnt = 0;
	} 
	void update() ;
	std::wstring toString() { return L"FibonacciAgent"; }


} ;