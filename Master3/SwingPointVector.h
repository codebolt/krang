#pragma once

#include "PlotOverlayTransform.h"
#include "SyncValue.h"

#include <vector>

class SwingPointVector : public PlotOverlayTransform {
	std::vector<SyncValue> spvec ;
	double threshold ;	// e.g. a high has to be >low*(1+threshold) to be considered a swing point
	int dir ;

	void initial() ;

public:
	enum { UP, DOWN } ;

	SwingPointVector(const CandlestickSeries *_psrc, double _threshold) : PlotOverlayTransform(_psrc), threshold(_threshold) { }
	const std::vector<SyncValue>& getVector() const { return spvec; }
	void update() ;
	void draw(HDC hDC, double hhigh, double llow, unsigned h, unsigned w, unsigned cnt0, unsigned cnt1) const ;
	int getDirection() const { return dir; }
	bool empty() { return spvec.empty(); }
} ;