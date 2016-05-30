#include "MovingAverageTransform.h"
#include "CandlestickSeries.h"
#include "Candlestick.h"
#include "Plot.h"

void MovingAverageTransform::update() {
	if(mavec.empty()) initial() ;
	else if(mavec.back().cscnt < psrc->back().cnt) updateNew() ;
	else updateCurrent() ;
}

void MovingAverageTransform::updateNew() {
	const Candlestick &cs = psrc->back() ;
	SyncValue val = { cs.cnt, mavec.back().value } ;

	val.value -= oldVal ;
	val.value += (prevVal = cs.close * factor) ;

	mavec.push_back(val) ;
	oldVal = psrc->at(++oldN).close * factor;
}

void MovingAverageTransform::updateCurrent() {
	SyncValue &val = mavec.back() ;
	double newClose = psrc->back().close ;

	val.value -= prevVal ;
	val.value += (prevVal = newClose * factor) ;
}

void MovingAverageTransform::initial() {
	// make the initial calculation
	if( psrc->size() != intervals ) return ;

	SyncValue val = { psrc->back().cnt, 0 } ;

	for(unsigned i=0;i<intervals;i++) {
		val.value += psrc->at(i).close * factor ;
	}

	mavec.push_back(val) ;

	prevVal = psrc->at(intervals-1).close * factor ;
	oldVal = psrc->at(0).close * factor ;
	oldN = 0 ;
}

void MovingAverageTransform::draw(HDC hDC, double hhigh, double llow, unsigned int h, unsigned int w, unsigned int cnt0, unsigned int cnt1) const {
	if(mavec.empty()) return ;

	unsigned cnt_from,cnt_to ;
	cnt_from = max( cnt0, mavec.front().cscnt ) ;
	cnt_to = min( mavec.back().cscnt, cnt1 ) ;

	unsigned offset = mavec.front().cscnt - psrc->begin()->cnt ;

	unsigned i0=cnt_from - offset ;
	unsigned i1=cnt_to - offset ;

	if(i0 >= (i1-2)) return ;	// nothing to draw

	for(unsigned i=i0+1; i<=i1; i++) {
		const SyncValue &prev = mavec[i-1] ;
		const SyncValue &next = mavec[i] ;

		int x0 = (Plot::translateX(i+offset-1-cnt0,w)+Plot::translateX(i+offset-cnt0,w))/2 ;
		int x1 = (Plot::translateX(i+offset-cnt0,w)+Plot::translateX(i+offset-cnt0+1,w))/2 ;
		int y0 = Plot::translateY(prev.value, hhigh, llow, h) ;
		int y1 = Plot::translateY(next.value, hhigh, llow, h) ;

		if(y0 == y1) 
			y1=y1;

		MoveToEx(hDC, x0, y0, NULL) ;
		LineTo(hDC, x1, y1) ;
	}
}
