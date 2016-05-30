#pragma once

#include "Agent.h"
#include "SwingPointVector.h"
#include "HeadShoulderTransform.h"

class HeadShoulderAgent : public Agent {
	SwingPointVector spvec ;
	HeadShoulderTransform hst ;

	void update() ;

public:
	HeadShoulderAgent(const CandlestickSeries *_psrc, double spthreshold)  ;
	std::wstring toString() { return L"HeadShoulderAgent"; }
} ;