#include "VolumePlot.h"
#include "GraphWindow.h"
#include "SimException.h"
#include "Candlestick.h"
#include "CandlestickSeries.h"

#include "boost/date_time/posix_time/posix_time.hpp"

void VolumePlot::drawPlot() {
	const GraphWindow *pgwnd = GraphWindow::getSingleton() ;
	const CandlestickSeries *pcss = pgwnd->getSelectedSeries() ;

	const unsigned i0 = pgwnd->getScrollPos() ;
	unsigned size=pcss->size();
	unsigned long highVol = 0;

	unsigned i=0;
	while( (i!=PLOT_STEPS) && ((i+i0)!=size) ) {
		const Candlestick& cs = pcss->at(i0+i);
		highVol = max(highVol, cs.volume) ;
		++ i ;
	}

	HBRUSH hbr = (HBRUSH) GetStockObject( DKGRAY_BRUSH );
	i = 0;
	while( (i!=PLOT_STEPS) && ((i+i0)!=size) ) {
		const Candlestick& cs = pcss->at(i+i0) ;
		
		bool up = cs.close > cs.open ;

		unsigned x0 = w*i/PLOT_STEPS ;
		unsigned x1 = x0 + w/PLOT_STEPS ;
		unsigned y0 = translateY(0, highVol, 0, h);
		unsigned y1 = translateY(cs.volume, highVol, 0, h);

		RECT rc = { x0, y1, x1, y0 } ;
		FillRect(hMemDC, &rc, hbr);

		++ i ;
	}
}

void VolumePlot::setText(unsigned offset) {
	const GraphWindow *pgwnd = GraphWindow::getSingleton() ;
	const CandlestickSeries *pcss = pgwnd->getSelectedSeries() ;

	unsigned scrollPos = pgwnd->getScrollPos();

	if(scrollPos+offset<pcss->size()) {
		const Candlestick &cs = pcss->at(scrollPos+offset) ;

		text = L"\r\nvolume: " + x2s(cs.volume) ;
	}
	else {
		text = L"";
	}
}
