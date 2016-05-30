#pragma once

#include <vector>

#include "Transform.h"
#include "SyncValue.h"

class RsiTransform : public Transform {
	unsigned intervals ;
	std::vector<SyncValue> vec ;

	double calc() ;	// compute the rsi

public:
	RsiTransform(const CandlestickSeries* _psrc, unsigned _intervals) : Transform(_psrc), intervals(_intervals) { }
	void update() ;
	double latest() { return vec.back().value ; }
	bool empty() { return vec.empty() ; }

	const std::vector<SyncValue>& getVec() { return vec; }
} ;