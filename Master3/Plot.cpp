#include "CandlestickPlot.h"
#include "Simulation.h"
#include "Candlestick.h"
#include "GraphWindow.h"

Plot::Plot(bool _top) : hWnd(0), top(_top) {

}

Plot::~Plot() {
	DeleteObject(hBmp);
	DeleteDC(hMemDC);
}

void Plot::init() {
	hWnd = GraphWindow::getHandle() ;
	RECT rc;
	GetClientRect(hWnd, &rc);
	w=rc.right;
	h=rc.bottom/2;
	HDC hDC = GetDC(hWnd);
	hMemDC = CreateCompatibleDC(hDC);
	hBmp = CreateCompatibleBitmap(hDC, w, h);
	ReleaseDC(hWnd, hDC);

	SelectObject(hMemDC, hBmp) ;
}

void Plot::resize(unsigned int _w, unsigned int _h) {
	if(!hWnd) {
		w=_w; h=_h;
		init() ;
		redraw() ;
	}
	else {
		HBITMAP hOldBmp = hBmp ;

		w=_w; h=_h;

		HDC hDC = GetDC(hWnd);
		hBmp = CreateCompatibleBitmap(hDC, w, h);
		ReleaseDC(hWnd, hDC);

		SelectObject(hMemDC, hBmp);
		DeleteObject(hOldBmp);

		redraw() ;
	}
}

void Plot::redraw() {
	if(!hWnd) init() ;

	RECT rc = { 0, 0, w, h };

	SetBkMode(hMemDC, TRANSPARENT);
	SetTextColor(hMemDC, RGB(0xff,0xff,0xff));

	// clear the DC
	FillRect(hMemDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

	// draw the text
	drawText() ;

	// draw the plot
	if(Simulation::getSingleton()->initialized()) {
		// only draw if the simulation is done initializing
		drawPlot();
	}
}

void Plot::blt(HDC hDC) const {
	int y = top ? 0 : h ;
	BitBlt(hDC, 0, y, w, h-1, hMemDC, 0, 0, SRCCOPY);
}

void Plot::mouse(unsigned int x) {
	// im too stupid to make this any less contrived right now, 6am

	for(int i=0;i<PLOT_STEPS;i++) {
		unsigned x0 = w*i/PLOT_STEPS ;
		unsigned x1 = x0 + w/PLOT_STEPS ;
		if( (x>=x0) && (x<=x1) ) {
			setText(i);
			break ;
		}
	}

	drawText() ;
}

void Plot::drawText() {
	// draw the text
	RECT rc;
	rc.top = 5;
	rc.left = 5;
	rc.right = w-5;
	rc.bottom = TOP_MARGIN - 2 ;

	FillRect(hMemDC, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH)) ; 

	if(text.length()>0) {
		HPEN hOldPen = (HPEN)SelectObject(hMemDC, GetStockObject(WHITE_PEN));
		DrawText(hMemDC, text.c_str(), text.length(), &rc, DT_LEFT);
		SelectObject(hMemDC, hOldPen);
	}
}