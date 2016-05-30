#include "HeadShoulderTransform.h"
#include "CandlestickSeries.h"
#include "SwingPointVector.h"
#include "Plot.h"

void HeadShoulderTransform::update() {
	const std::vector<Candlestick> &csvec = psrc->getVec() ;
	const Candlestick& cs = csvec.back() ;
	const unsigned MAXAGE = 100 ;

	if(cs.cnt < 7) return ;

	if(prevcscnt != cs.cnt) {
		// new candle, look for new candidate patterns
//		assert(prevcscnt == cs.cnt-1) ;
		findPatterns() ;
		prevcscnt = cs.cnt ;
	}

	// remove old patterns
	while( (!candidates.empty()) && (candidates.front().sv[0].cscnt+MAXAGE < cs.cnt) ) {
		candidates.pop_front() ; 
	}

	matches = 0;
	double prevhigh = (csvec.end()-2)->high ;
	for( std::deque<HeadShoulderPattern>::const_iterator it = candidates.begin(); it != candidates.end(); it++ ) {
		if( necklineBreak( *it, cs, prevhigh ) ) {
			matches ++ ;
		}
	}
}

void HeadShoulderTransform::findPatterns() {
	const unsigned MAXCNTDIFF = 45 ;	// pattern has to be contained in the last N candles
	const unsigned MAXSP = 16 ;			// max amount of swing points
	const std::vector<SyncValue> &spv = spvec.getVector() ;
	const unsigned size = spv.size() ;

	if(size < 7) return ;	// since a pattern is made up of 7 swing points, we need at least this amount
	else if( spvec.getDirection() != SwingPointVector::DOWN ) return ;	// only look when direction is down

	const Candlestick& lastcs = psrc->getVec().back() ;
	const SyncValue& lastsp = spv.back() ;

	if(lastsp.cscnt != lastcs.cnt) return ;	// no need to look for new patterns if there are no new swing points

	unsigned i0 = 0 ;
	while(spv[i0].cscnt+MAXCNTDIFF < lastcs.cnt) i0 ++ ;	// i0 = index of oldest swing point to consider
	if(i0 >= size-7) return ;
	unsigned imin = size>MAXSP?size-MAXSP:0 ;
	i0 = max(i0, imin) ;
	if(spv[i0].value < spv[i0+1].value) i0 ++ ;				// make sure i0 is a high point
//
	// check that a new swing point has occured (no need to test if we already have)
	if(spv[size-2].cscnt == prevspcnt) return ;
	prevspcnt = spv[size-2].cscnt ;

	// iterate
	unsigned t2,p2,t1,p1,t0,p0 ;
	for( p0=i0 ;p0<(size-7);p0+=2)
		for(t0=p0+1;t0<(size-6);t0+=2)
			for(p1=t0+1;p1<(size-5);p1+=2)
				for(t1=p1+1;t1<(size-4);t1+=2)
					for(p2=t1+1;p2<(size-3);p2+=2)
						for(t2=p2+1;t2<(size-2);t2+=2)
							testPattern( spv[p0], spv[t0], spv[p1], spv[t1], spv[p2], spv[t2], spv[size-2] ) ;
}

void HeadShoulderTransform::testPattern(const SyncValue& p0, const SyncValue& t0, const SyncValue& p1, const SyncValue& t1,
										const SyncValue& p2, const SyncValue& t2, const SyncValue& p3) 
{
	// test if the candlesticks make a hs-candidate
	// based on Zapranis & Samolada: http://www.springerlink.com/content/c388328442n26v74/
	if( (p2.value > p1.value && p2.value > p3.value)	// shs1: head > shoulders
	 && (p1.value > p0.value && t1.value > t0.value)	// shs2: upward trend
	 && ((p1.value > 0.5*(p3.value+t2.value)) && (p3.value > 0.5*(p1.value+t1.value)))	// shs3: balance
	 && ((p2.cscnt-p1.cscnt) < 3*(p3.cscnt-p2.cscnt)) && ((p3.cscnt-p2.cscnt) < 3*(p2.cscnt-p1.cscnt)) )	// shs4: symmetry
	{
		 HeadShoulderPattern hsp = { p0, t0, p1, t1, p2, t2, p3 } ;
		 candidates.push_back(hsp);
		 patterns.push_back(hsp);
	}
}

bool HeadShoulderTransform::necklineBreak(const HeadShoulderPattern &hsp, const Candlestick &cs, double prevhigh) {
	if(cs.cnt < (hsp.sv[6].cscnt + hsp.sv[6].cscnt - hsp.sv[2].cscnt)) {
		double delta_t = hsp.sv[5].cscnt - hsp.sv[3].cscnt ;
		double delta_p = hsp.sv[5].value - hsp.sv[3].value ;
		double nc_p = hsp.sv[3].value + (delta_p/delta_t) * (double)(cs.cnt - hsp.sv[3].cscnt) ;
		if( (cs.close < nc_p) && (prevhigh > nc_p) ) return true ;
	}

	return false ;
}

void HeadShoulderTransform::draw(HDC hDC, double hhigh, double llow, unsigned int h, unsigned int w, unsigned int cnt0, unsigned int cnt1) const {
#define IN_RANGE(cnt) ((cnt >= cnt0) && (cnt <= cnt1))
	HPEN hOldPen = (HPEN) SelectObject(hDC, GetStockObject(WHITE_PEN)) ;
	for(std::vector<HeadShoulderPattern>::const_iterator it = patterns.begin(); it != patterns.end(); it++) {
		for(int i=0;i<HeadShoulderPattern::PATTERN_SIZE-1;i++) {
			unsigned cntA = it->sv[i].cscnt ;
			if(IN_RANGE(cntA)) {
				// draw the line between point A (i) and B (i+1)
				unsigned x0 = Plot::translateX( it->sv[i].cscnt - cnt0, w ) ;
				unsigned x1 = Plot::translateX( it->sv[i+1].cscnt - cnt0, w ) ;
				unsigned y0 = Plot::translateY( it->sv[i].value, hhigh, llow, h ) ;
				unsigned y1 = Plot::translateY( it->sv[i+1].value, hhigh, llow, h ) ;
				MoveToEx(hDC, x0, y0, NULL);
				LineTo(hDC, x1, y1) ;
			}
		}
	}
	SelectObject(hDC, hOldPen) ;
#undef IN_RANGE
}