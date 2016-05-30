#pragma once

#include "Plot.h"
#include "Transform.h"

class PlotTransform : public Plot, public Transform {
public:
	PlotTransform(const CandlestickSeries *_psrc) ;
};