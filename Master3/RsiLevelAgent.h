#pragma once

#include "RsiTransform.h"
#include "Agent.h"

class RsiLevelAgent : public Agent {
	RsiTransform transform ;

public:
	RsiLevelAgent(const CandlestickSeries *_psrc, unsigned intervals) : Agent(), transform(_psrc, intervals) { }
	void update() ;
	std::wstring toString() { return L"RsiLevelAgent" ; }
} ;