#pragma once

#include "Plot.h"

class CandlestickSeries ;

class CandlestickPlot : public Plot {
	void drawPlot();
	void setText(unsigned offset);

public:
	CandlestickPlot() : Plot(true) { } ;
} ;