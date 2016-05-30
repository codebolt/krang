#pragma once

#include "Plot.h"

class VolumePlot : public Plot {
	void drawPlot();
	void setText(unsigned offset);

public:
	VolumePlot() : Plot(false) { } ;
} ;