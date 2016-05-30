#include "Agent.h"
#include "CandlestickSeries.h"
#include "Candlestick.h"
#include "GraphWindow.h"

void Agent::update(const boost::posix_time::ptime &time) {
	double prevSignal = signal ;
	this->update();
	if(prevSignal != signal) {
		// save the new signal
		SignalRecord sr = { time, signal } ;
		signalHistory.push_back(sr) ;
	}
}

void Agent::drawPlot() {
	if(signalHistory.empty()) return ;

	const GraphWindow *pgwnd = GraphWindow::getSingleton() ;
	const CandlestickSeries *pcss = pgwnd->getSelectedSeries() ;

	const unsigned i0 = pgwnd->getScrollPos() ;
	unsigned size=pcss->size(), shsize=signalHistory.size() ;
	double hhigh=1.0, llow=-1.0 ;

	unsigned i = 0;
	unsigned shi = 0;	// signal history index

	boost::posix_time::ptime cst0 = pcss->at(i0).tp.begin() ;
	std::vector<SignalRecord>::const_iterator shit = signalHistory.begin() ;	// shit=signal history iterator (profanity not intended)

	while( (shit!=signalHistory.end()) && (shit->pt < cst0) ) shit ++ ;	// skip to the first relevant record of the graph

	// construct each candle in the graph
	HBRUSH hbrUp = CreateSolidBrush( RGB(60,228,242) );
	HBRUSH hbrDown = CreateSolidBrush( RGB(228,72,201) );
	HPEN hWhitePen = (HPEN) GetStockObject( WHITE_PEN );
	HPEN hFlatPen = CreatePen(PS_DOT, 1, RGB(0x50,0x50,0x50));
	HPEN hOldPen = (HPEN) SelectObject(hMemDC, hWhitePen);
	double open,close,high,low ;

	if(shit != signalHistory.begin()) open = high = low = close = (shit-1)->signal ;
	else open = high = low = close = 0.0 ;

	for(unsigned i=0;i<min(PLOT_STEPS,pcss->size());i++) {
		const boost::posix_time::time_period &period = pcss->at(i0+i).tp ;

		while( (shit!=signalHistory.end()) && (shit->pt < period.begin()) ) shit ++ ;

		// get the values of the candle
		if( (shit != signalHistory.end()) && (period.contains(shit->pt)) ) {
			close = low = high = open = shit->signal ;
			do {
				if(shit->signal < low) low = shit->signal ;
				if(shit->signal > high) high = shit->signal ;
				close = shit->signal ;
				shit ++ ;
			} while((shit!=signalHistory.end())&&period.contains(shit->pt)) ;
		}
		else {
			open = low = high = close ;
		}

		// calculate pixel coordinates and draw the candle
		unsigned yclose = translateY(close, hhigh, llow, h) ;
		unsigned yopen = translateY(open, hhigh, llow, h) ;
		unsigned ylow = translateY(low, hhigh, llow, h);
		unsigned yhigh = translateY(high, hhigh, llow, h);

		labels[i] = toString() + L" signals\r\nopen:"+x2s(open)+L"   low:"+x2s(low)+L"   high:"+x2s(high)+L"   close:"+x2s(close) ;

		unsigned x0 = translateX(i, w) ;
		unsigned x1 = translateX(i+1, w) ;

		if(ylow != yhigh) {
			SelectObject(hMemDC, hWhitePen);
			MoveToEx(hMemDC, (x0+x1)/2, ylow, NULL);
			LineTo(hMemDC, (x0+x1)/2, yhigh);
		}

		if(yopen == yclose) {
			SelectObject(hMemDC, hFlatPen);
			MoveToEx(hMemDC, x0, yclose, NULL) ;
			LineTo(hMemDC, x1, yclose) ;
		}
		else {
			RECT rc ;
			rc.bottom = max(yclose, yopen) ;
			rc.top = min(yclose, yopen) ;
			rc.left = x0 ;
			rc.right = x1 ;
			FillRect(hMemDC, &rc, (close>open)?hbrUp:hbrDown) ;
		}
	}

	// clean up gdi objects
	SelectObject(hMemDC, hOldPen);
	DeleteObject(hbrUp);
	DeleteObject(hbrDown);
	DeleteObject(hFlatPen);
}

void Agent::setText(unsigned int offset) {
	assert(offset < PLOT_STEPS) ;
	this->text = labels[offset] ;
}