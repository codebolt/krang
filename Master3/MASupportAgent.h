#pragma once

#include "Agent.h"
#include "MovingAverageTransform.h"

class MASupportAgent : public Agent {
private:
	MovingAverageTransform transform ;
	const CandlestickSeries* psrc ;
	double threshold ;

	void update() ;

public:
	MASupportAgent(const CandlestickSeries* _psrc, unsigned intervals, double _threshold) ;
	std::wstring toString() { return L"MASupportAgent"; }
} ;