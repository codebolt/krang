#pragma once

#include "Agent.h"

#include "SwingPointVector.h"

class DoubleTopAgent : public Agent {
	SwingPointVector spv ;
	double pthresh ;

public:
	DoubleTopAgent(const CandlestickSeries *psrc, double spthresh, double _pthresh) : Agent(), spv(psrc, spthresh), pthresh(_pthresh) { }
	void update();

	std::wstring toString() { return L"DoubleTopAgent" ; }
} ;