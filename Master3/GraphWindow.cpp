#include "GraphWindow.h"
#include "Simulation.h"
#include "CandlestickPlot.h"
#include "VolumePlot.h"
#include "Candlestick.h"
#include "CandlestickSeries.h"
#include "MainDlg.h"
#include "Agent.h"

#include "resource.h"

GraphWindow* GraphWindow::singleton = 0;

#define MENUID_SERIES 0x1000 
#define MENUID_COPLOT 0x2000

GraphWindow::~GraphWindow() {
	delete pcsplot ;
	delete pvolplot ;
}

LRESULT CALLBACK GraphWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	GraphWindow* wnd = GraphWindow::getSingleton();

	switch(uMsg) {
	case WM_CREATE:
		wnd->onCreate(hWnd);
		return 0l;

	case WM_PAINT:
		wnd->onPaint();
		return 0l;

	case WM_SIZE:
		wnd->onSize(LOWORD(lParam),HIWORD(lParam));
		return 0l;

	case WM_HSCROLL:
		wnd->onHScroll(LOWORD(wParam));
		return 0l;

	case WM_MOUSEMOVE:
		wnd->onMouseMove(LOWORD(lParam));
		return 0l;

	case WM_COMMAND:
		if(!HIWORD(wParam)) {
			// from a menu
			wnd->onMenu(LOWORD(wParam));
		}
		return 0l;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

bool GraphWindow::create(HINSTANCE hInstance) {
	const wchar_t szClassName[] = L"GraphWindow" ;
	const wchar_t szWindowName[] = L"Real Time Simulation Data";

	WNDCLASSEX wcex ;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hCursor = LoadCursor(0, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH );
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = GraphWndProc;
	wcex.lpszClassName = szClassName;
	wcex.lpszMenuName = 0 ;
	wcex.style = CS_VREDRAW | CS_HREDRAW;

	if( !RegisterClassEx(&wcex) ) {
		return false ;
	}

	singleton = new GraphWindow() ;

	HWND hWnd = CreateWindowEx(0, szClassName, szWindowName, WS_OVERLAPPEDWINDOW|WS_HSCROLL, 480, 10, 640, 480, 0, CreateMenu(), hInstance, 0);
	ShowWindow(hWnd, SW_SHOWNORMAL);

	return true ;
}

void GraphWindow::onCreate(HWND _hWnd)
{
	hWnd = _hWnd ;
	hMenu = GetMenu(hWnd);

	pcsplot = new CandlestickPlot() ;
	selplot = pvolplot = new VolumePlot() ;

	initMenu() ;
}

void GraphWindow::onMouseMove(unsigned int xpos) {
	// TODO
	pcsplot->mouse(xpos) ;
	selplot->mouse(xpos) ;
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd) ;
}

void GraphWindow::onHScroll(unsigned wType) {
	// i hate winapi
	SCROLLINFO si = { 0 } ;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(hWnd,SB_HORZ,&si);
	switch(wType) {
	case SB_LINELEFT:
		si.nPos -= 1 ; break;
	case SB_LINERIGHT:
		si.nPos += 1 ; break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage ; break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage ; break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos ; break;
	}
	SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	update() ;
}

void GraphWindow::onSize(unsigned w, unsigned h) {
	pcsplot->resize(w,h/2);
	selplot->resize(w,h/2);
}

void GraphWindow::update() {
	// update the scroll bar
	const Simulation* psim = Simulation::getSingleton() ;
	
	if(psim->initialized()) {
		updateScrollInfo();
		pcsplot->redraw();
		selplot->redraw();
		InvalidateRect(hWnd,NULL,FALSE);
	}

	UpdateWindow(hWnd);
}

void GraphWindow::onPaint() {
	const Simulation *psim = Simulation::getSingleton() ;
	PAINTSTRUCT ps ;
	RECT rc ;
	GetClientRect(hWnd, &rc);
	HDC hDC = BeginPaint(hWnd, &ps);

	if(psim->initialized()) {
		pcsplot->blt(hDC);
		selplot->blt(hDC);
	}

	// paint the separating line
	HPEN hOldPen = (HPEN) SelectObject(hDC, CreatePen(PS_SOLID, 3, RGB(0x3f,0x3f,0xaf)));
	MoveToEx(hDC, 0, rc.bottom/2, NULL);
	LineTo(hDC, rc.right, rc.bottom/2);
	DeleteObject(SelectObject(hDC, hOldPen));

	EndPaint(hWnd, &ps);
}

void GraphWindow::updateScrollInfo() {
	const Simulation* psim = Simulation::getSingleton() ;
	const CandlestickSeries* pcss = getSelectedSeries() ;

	unsigned size = pcss->size() ;

	SCROLLINFO si0 = { 0 } ;	// current state
	SCROLLINFO si1 = { 0 } ;	// new state

	si0.cbSize = sizeof(SCROLLINFO);
	si0.fMask = SIF_RANGE | SIF_POS;
	GetScrollInfo(hWnd, SB_HORZ, &si0);

	si1.cbSize = sizeof(SCROLLINFO) ;
	si1.fMask = SIF_DISABLENOSCROLL | SIF_RANGE | SIF_POS | SIF_PAGE ;
	si1.nMax = (size>Plot::PLOT_STEPS)?(size-Plot::PLOT_STEPS):0 ;
	si1.nPos = (psim->running())?si1.nMax:si0.nPos ;
	si1.nPage = min(10, si1.nMax) ;
	SetScrollInfo(hWnd, SB_HORZ, &si1, TRUE);
}

unsigned GraphWindow::getScrollPos() const {
	SCROLLINFO si = { 0 } ;
	si.cbSize = sizeof(SCROLLINFO) ;
	si.fMask = SIF_POS ;
	GetScrollInfo(hWnd, SB_HORZ, &si);
	return si.nPos;
}

void GraphWindow::initMenu() {
	using namespace std ;

	const Simulation *psim = Simulation::getSingleton() ;

	// destroy any existing co-plot menu
	HMENU hCssMenu = CreatePopupMenu() ;	// Candle series
	HMENU hCpMenu = CreatePopupMenu() ;		// Co-plot

	InsertMenu(hMenu, 0, MF_POPUP | MF_STRING, (UINT_PTR) hCssMenu, L"Series");
	InsertMenu(hMenu, 1, MF_POPUP | MF_STRING, (UINT_PTR) hCpMenu, L"Co-plot");

	const vector< CandlestickSeries* > &cssvec = psim->getSeries() ;
	selcss = cssvec.front() ;

	unsigned i=0 ;

	MENUITEMINFO mii = { 0 } ;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING | MIIM_DATA | MIIM_STATE | MIIM_ID ;

	for( vector<CandlestickSeries*>::const_iterator it = cssvec.begin(); it != cssvec.end(); it++ ) {
		// populate the css menu
		std::wstring cssName = (*it)->toString() ;
		mii.dwTypeData = (LPWSTR)cssName.c_str() ;
		mii.dwItemData = (ULONG_PTR)*it ;
		mii.fState = (**it) == (*selcss) ? MFS_CHECKED : 0 ;
		mii.wID = MENUID_SERIES + i ;
		InsertMenuItem(hCssMenu, i, TRUE, &mii) ;
		++i;
	}

	// insert volume co-plot
	selplot = pvolplot ;	// set the selected plot to volume plot
	mii.dwTypeData = L"Volume" ;
	mii.dwItemData = (ULONG_PTR) pvolplot ;
	mii.fState = MFS_CHECKED ;
	mii.wID = MENUID_COPLOT ;
	InsertMenuItem(hCpMenu, 0, TRUE, &mii) ;

	updateMenu() ; // inserts agent co-plots

	DrawMenuBar(hWnd);

	updateScrollInfo() ;
}

void GraphWindow::updateMenu() {
	using namespace std;

	// update agent co-plots
	HMENU hCpMenu = GetSubMenu(hMenu, 1);

	int n = GetMenuItemCount(hCpMenu) ;
	while( n > 1 ) {
		RemoveMenu(hCpMenu, --n, MF_BYPOSITION) ;
	}

	MENUITEMINFO mii = { 0 } ;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING | MIIM_DATA | MIIM_ID ;

	unsigned i = 1 ;	// 0 is for the volume plot
	const vector<Agent*> &agentvec = selcss->getAgents();
	for( vector<Agent*>::const_iterator it = agentvec.begin(); it != agentvec.end(); it++) {
		std::wstring agentName = (*it)->toString() ;
		mii.dwTypeData = (LPWSTR)agentName.c_str() ;
		mii.dwItemData = (ULONG_PTR) *it ;
		mii.wID = MENUID_COPLOT + i;
		InsertMenuItem(hCpMenu, i, TRUE, &mii) ;
		++i ;
	}
}

void GraphWindow::onMenu(unsigned int id) {
	if( (id & 0xf000) == MENUID_SERIES ) {
		// selected a new candlestick series

		HMENU hCssMenu = GetSubMenu(hMenu,0);
		int cnt = GetMenuItemCount( hCssMenu ) ;
		int pos = id & 0x0fff ;

		// uncheck current selection
		for(int i=0;i<cnt;i++) {
			CheckMenuItem(hCssMenu, i, MF_BYPOSITION | MF_UNCHECKED) ;
		}

		// check the new selection
		CheckMenuItem(hCssMenu, pos, MF_BYPOSITION | MF_CHECKED) ;	// the position is hacked into the id
		selcss = (const CandlestickSeries*) GetMenuItemPtr(hCssMenu,pos) ;
		updateScrollInfo() ;
		updateMenu() ;
		pcsplot->redraw() ;
	}
	else if( (id & 0xf000) == MENUID_COPLOT ) {
		// selected a new co-plot

		HMENU hCpMenu = GetSubMenu(hMenu,1);
		int cnt = GetMenuItemCount(hCpMenu);
		int pos = id & 0x0fff ;

		for(int i=0;i<cnt;i++) CheckMenuItem(hCpMenu,i,MF_BYPOSITION|MF_UNCHECKED) ;

		RECT rc ;
		GetClientRect(hWnd, &rc);
		CheckMenuItem(hCpMenu, pos, MF_BYPOSITION|MF_CHECKED) ;
		selplot = (Plot*) GetMenuItemPtr(hCpMenu,pos) ;
		selplot->resize(rc.right, rc.bottom/2);
	}

	DrawMenuBar(hWnd) ;

	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

ULONG_PTR GraphWindow::GetMenuItemPtr(HMENU hSubMenu, int pos) {
	MENUITEMINFO mii = {0} ;
	mii.cbSize = sizeof(MENUITEMINFO) ;
	mii.fMask = MIIM_DATA ;
	if( ! GetMenuItemInfo(hSubMenu, pos, MF_BYPOSITION, &mii) ) throw SIM_EX ( L"Problem with GetMenuItemInfo" ) ;
	return mii.dwItemData ;
}