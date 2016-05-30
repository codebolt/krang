#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CandlestickPlot ;
class VolumePlot ;
class Simulation ;
class CandlestickSeries ;
class Plot ;

class GraphWindow {
	static GraphWindow *singleton ;

	HWND hWnd ;
	HMENU hMenu ;

	const CandlestickSeries *selcss ;

	CandlestickPlot *pcsplot;
	VolumePlot *pvolplot;
	Plot *selplot ;

	GraphWindow() : pcsplot(0), selcss(0) { }
	~GraphWindow() ;

	void updateScrollInfo() ;

	void initMenu() ;
	void updateMenu() ;

	ULONG_PTR GetMenuItemPtr(HMENU hSubMenu, int pos) ;

public:
	const CandlestickSeries* getSelectedSeries() const { return selcss; } // returns the currently selected series

	// initialize the singleton
	static bool create(HINSTANCE hInstance) ;

	// get the singleton
	static GraphWindow* getSingleton() { return singleton; }
	static HWND getHandle() { return singleton->hWnd; }

	// redraw the window
	void update();

	// message handlers
	void onCreate(HWND hWnd);
	void onSize(unsigned w, unsigned h) ;
	void onPaint() ;
	void onHScroll(unsigned wType) ;
	void onMouseMove(unsigned xpos) ;
	void onMenu(unsigned id) ;

	unsigned getScrollPos() const ;
};