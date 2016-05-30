#include "GraphWindow.h"
#include "CandlestickPlot.h"
#include "CandlestickSeries.h"
#include "Simulation.h"
#include "SimException.h"
#include "Candlestick.h"
#include "Agent.h"
#include "PlotOverlayTransform.h"

#include "boost/date_time/posix_time/posix_time.hpp"

void CandlestickPlot::drawPlot()
{
	const GraphWindow *pgwnd = GraphWindow::getSingleton() ;
	const CandlestickSeries *pcss = pgwnd->getSelectedSeries() ;

	const unsigned i0 = pgwnd->getScrollPos() ;
	unsigned size=pcss->size();
	double hhigh=0, llow=1E16 ;

	unsigned i=0;
	while( (i!=PLOT_STEPS) && ((i+i0)!=size) ) {
		const Candlestick& cs = pcss->at(i0+i);
		hhigh = max(hhigh,cs.high);
		llow = min(llow,cs.low);
		++ i ;
	}

	HBRUSH hbrUp = CreateSolidBrush( RGB(0x2f,0xff,0) );
	HBRUSH hbrDown = CreateSolidBrush( RGB(0xff,0x2f,0) );

	HPEN hWhitePen = (HPEN) GetStockObject( WHITE_PEN );
	HPEN hFlatPen = CreatePen(PS_DOT, 1, RGB(0x50,0x50,0x50));
	HPEN hOldPen = (HPEN) SelectObject(hMemDC, hWhitePen);

	i = 0;
	while( (i!=PLOT_STEPS) && ((i+i0)!=size) ) {
		const Candlestick& cs = pcss->at(i+i0) ;
		
		bool up = cs.close > cs.open ;

		unsigned x0 = w*i/PLOT_STEPS ;
		unsigned x1 = x0 + w/PLOT_STEPS ;
		unsigned yOpen = translateY(cs.open, hhigh, llow, h);
		unsigned yClose = translateY(cs.close, hhigh, llow, h);
		unsigned yHigh = translateY(cs.high, hhigh, llow, h);
		unsigned yLow = translateY(cs.low, hhigh, llow, h);


		if(yLow != yHigh) {
			SelectObject(hMemDC, hWhitePen);
			MoveToEx(hMemDC, (x0+x1)/2, yLow-1, NULL);
			LineTo(hMemDC, (x0+x1)/2, yHigh);
		}

		if(yOpen == yClose) {
			SelectObject(hMemDC, hFlatPen);
			MoveToEx(hMemDC, x0, yClose, NULL);
			LineTo(hMemDC, x1, yClose);
		}
		else {
			RECT rc = { x0, up?yClose:yOpen, x1-1, up?yOpen:yClose } ;
			FillRect( hMemDC, &rc, up?hbrUp:hbrDown );
		}

		++ i ;
	}
	SelectObject(hMemDC, hOldPen);
	DeleteObject(hFlatPen);
	DeleteObject(hbrUp);
	DeleteObject(hbrDown);

	// TODO: add any relevant overlays here
	const CandlestickSeries::AgentVec& agents = pcss->getAgents();

	hOldPen = (HPEN) SelectObject(hMemDC, GetStockObject(WHITE_PEN)) ;

	for(CandlestickSeries::AgentVec::const_iterator agent=agents.begin(); agent!=agents.end(); agent++) {
		const Agent::TransformVec& tvec = (*agent)->getTransforms() ;

		for(Agent::TransformVec::const_iterator transform = tvec.begin(); transform!=tvec.end(); transform++) {
			const PlotOverlayTransform *overlay = dynamic_cast<const PlotOverlayTransform*>( *transform ) ;
			if(!!overlay) {
				// plot overlay
				overlay->draw(hMemDC, hhigh, llow, h, w, i0, i+i0-1) ;
			}
		}
	}
	SelectObject(hMemDC, hOldPen);
}

void CandlestickPlot::setText(unsigned offset) {
	const GraphWindow *pgwnd = GraphWindow::getSingleton() ;
	const CandlestickSeries *pcss = pgwnd->getSelectedSeries() ;

	if(pcss->empty()) {
		text = L"" ;
		return ;
	}

	unsigned scrollPos = pgwnd->getScrollPos();

	unsigned index = min(scrollPos+offset, pcss->size()-1);

	const Candlestick &cs = pcss->at(index) ;

	text = boost::posix_time::to_simple_wstring(cs.tp)
		+ L"\r\nopen:" + x2s(cs.open) + L"    low:" + x2s(cs.low) + L"    high:" + x2s(cs.high) + L"    close:" + x2s(cs.close) ;
}