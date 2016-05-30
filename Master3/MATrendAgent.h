#pragma once

#include "Agent.h"
#include "MATrendClassifier.h"

class MATrendAgent : public Agent {
private:
	MATrendClassifier matc ;
	const CandlestickSeries* psrc ;

	void update() ;

public:
	MATrendAgent(const CandlestickSeries* _psrc, unsigned longInt, unsigned shortInt) ;
	std::wstring toString() { return L"MATrendAgent"; }
} ;