#include "SwingPointVector.h"
#include "CandlestickSeries.h"
#include "Plot.h"

#include <cassert>

void SwingPointVector::update() {
	if(spvec.empty()) initial() ;
	else {
		const Candlestick& cs = psrc->getVec().back() ;
		SyncValue& sp = spvec.back();

		switch(dir) {
		case UP:
			if( cs.high>sp.value ) {
				sp.cscnt = cs.cnt ;
				sp.value = cs.high ;
			}
			else if( cs.low<sp.value*(1.0-threshold) && (sp.cscnt != cs.cnt) ) {
				SyncValue tmp = { cs.cnt, cs.low } ;
				spvec.push_back(tmp) ;
				dir = DOWN ;
			}
			break ;
		case DOWN:
			if( cs.low<sp.value ) {
				sp.cscnt = cs.cnt ;
				sp.value = cs.low ;
			}
			else if( cs.high>sp.value*(1.0+threshold) && (sp.cscnt != cs.cnt) ) {
				SyncValue tmp = { cs.cnt, cs.high } ;
				spvec.push_back(tmp) ;
				dir = UP ;
			}
			break;
		default:
			assert(0);	// something bad happened
		}
	}
}

void SwingPointVector::initial() {
	using namespace std ;

	// spvec is empty, look for the first swing point
	const vector<Candlestick> &csvec = psrc->getVec() ;
	vector<Candlestick>::const_iterator csit = csvec.begin() ;

	assert(!csvec.empty()) ;

	const Candlestick* plow = &*csit;
	const Candlestick* phigh = &*csit ;

	while((++csit) != csvec.end()) {
		if(plow->low > csit->low) plow = &*csit ;
		if(phigh->high < csit->high) phigh = &*csit ;
	}

	if(phigh->high > plow->low*(1.0+threshold)) {
		SyncValue svhi = { phigh->cnt, phigh->high } ;
		SyncValue svlo = { plow->cnt, plow->low } ;

		// add them in chronological order
		if(svhi.cscnt > svlo.cscnt) {
			spvec.push_back(svlo) ;
			spvec.push_back(svhi) ;
			dir = UP ;
		}
		else {
			spvec.push_back(svhi) ;
			spvec.push_back(svlo) ;
			dir = DOWN ;
		}
	}
}

void SwingPointVector::draw(HDC hDC, double hhigh, double llow, unsigned int h, unsigned int w, unsigned int cnt0, unsigned int cnt1) const {
	std::vector<SyncValue>::const_iterator spit = spvec.begin() ;
	const std::vector<Candlestick>& csvec = psrc->getVec() ;

	unsigned N = (1+cnt1-cnt0) ;
	for(unsigned i=0;(i<N)&&(spit!=spvec.end());i++) {
		const Candlestick& cs = csvec[cnt0+i] ;

		assert( cs.cnt == (cnt0+i) ) ;

		while((spit!=spvec.end()) && (spit->cscnt<cs.cnt)) {
			spit ++ ;
		}

		while((spit!=spvec.end()) && (spit->cscnt==cs.cnt)) {
			RECT rc ;
			rc.left = Plot::translateX(i,w) ;
			rc.right = Plot::translateX(i+1,w)-1 ;
			rc.top = Plot::translateY(spit->value, hhigh, llow, h) - (w/(2*Plot::PLOT_STEPS)) ;
			rc.bottom = rc.top + (w/Plot::PLOT_STEPS) ;

			HPEN hOldPen = (HPEN) SelectObject( hDC, CreatePen(PS_SOLID, 2, RGB(23,238,0xff)) ) ;

			MoveToEx(hDC, rc.left, rc.top, NULL);
			LineTo(hDC, rc.right, rc.bottom);
			MoveToEx(hDC, rc.left, rc.bottom, NULL);
			LineTo(hDC, rc.right, rc.top);

			DeleteObject( SelectObject(hDC, hOldPen) ) ;

			spit ++;
		}
	}
}

