#pragma once

#include "MovingAverageTransform.h"

class MATrendClassifier {
	MovingAverageTransform lma, mma, sma ;	// long, mid, short

public:
	MATrendClassifier(const CandlestickSeries *psrc, unsigned lint, unsigned mint, unsigned sint) :
	  lma(psrc, lint), mma(psrc, mint), sma(psrc, sint) { }

    void update() ;
	double trend() ;
} ;