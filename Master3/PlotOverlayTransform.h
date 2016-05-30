#pragma once

#include "Transform.h"
#include "PlotOverlay.h"

class PlotOverlayTransform : public PlotOverlay, public Transform {
public:
	PlotOverlayTransform(const CandlestickSeries* _psrc) : PlotOverlay(), Transform(_psrc) {}
} ;