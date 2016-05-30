#pragma once

#include "PlotOverlayTransform.h"
#include "SyncValue.h"

#include <vector>

class SvSwingPointVector {
	typedef std::vector<SyncValue> SvVec ;
	SvVec spvec ;
	const SvVec* psrc ;
	double threshold ;	// e.g. a high has to be >low*(1+threshold) to be considered a swing point
	int dir ;

	void initial() ;

public:
	enum { UP, DOWN } ;

	void initsrc(const SvVec* _psrc) { psrc = _psrc ; }
	SvSwingPointVector(double _threshold) : psrc(0), threshold(_threshold) { }
	const std::vector<SyncValue>& getVector() const { return spvec; }
	void update() ;
	int getDirection() const { return dir; }
} ;