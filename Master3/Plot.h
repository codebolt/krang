#pragma once

#include "WinInclude.h"
#include <string>

class Simulation ;

class Plot {
protected:

	static const unsigned TOP_MARGIN = 60 ;
	static const unsigned BOTTOM_MARGIN = 20 ;

	HWND hWnd ;
	HDC hMemDC ;	// memory dc
	HBITMAP hBmp ;
	unsigned w, h ;
	bool top ;

	std::wstring text ;	// drawn at the top (info on current highlight)

	void drawText() ;

	virtual void drawPlot()=0;
	virtual void setText(unsigned offset)=0;

public:
	Plot( bool _top ) ;
	~Plot( );

	void init() ;
	void resize( unsigned _w, unsigned _h ) ;
	void mouse( unsigned x ) ;	// mouse moved
	void redraw( ) ;
	void blt( HDC hDC ) const ;

	unsigned height() { return h; }

	static const unsigned PLOT_STEPS = 60 ;

	static unsigned translateY(double val, double hhigh, double llow, unsigned h) 
	{ 
		return TOP_MARGIN + (unsigned) ( ((hhigh-val)/(hhigh-llow)) * (double)(h-(TOP_MARGIN+BOTTOM_MARGIN)) ) ;
	}

	static unsigned translateX(unsigned i, unsigned w) {
		return (w*i)/PLOT_STEPS ;
	}
} ;