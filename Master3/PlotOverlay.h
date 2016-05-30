#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class PlotOverlay {
public:
	virtual void draw(HDC hDC, double hhigh, double llow, unsigned h, unsigned w, unsigned cnt0, unsigned cnt1) const = 0 ;
} ;
