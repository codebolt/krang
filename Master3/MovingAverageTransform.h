#pragma once

#include "PlotOverlayTransform.h"
#include "SyncValue.h"

#include <vector>

class MovingAverageTransform : public PlotOverlayTransform {
	std::vector<SyncValue> mavec ;
	double prevVal ;
	double oldVal ;
	unsigned oldN ;
	const unsigned intervals ;
	const double factor ;

	void initial() ;
	void updateCurrent() ;
	void updateNew() ;

public:
	MovingAverageTransform(const CandlestickSeries* _psrc, unsigned _intervals) : PlotOverlayTransform(_psrc), intervals(_intervals), factor(1/(double)_intervals) { }

	const std::vector<SyncValue>& getVector() { return mavec; }
	void update() ;
	void draw(HDC hDC, double hhigh, double llow, unsigned h, unsigned w, unsigned cnt0, unsigned cnt1) const ;
	bool empty() { return mavec.empty(); }
	double last() { return mavec.back().value; }
};

