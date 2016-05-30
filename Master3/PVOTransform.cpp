#include "PVOTransform.h"
#include "CandlestickSeries.h"

void PVOTransform::update() {
	const Candlestick& lastcs = psrc->back() ;

	if(psrc->size() == 1) {
		lvma = ltmp = svma = stmp = lastcs.volume ;
		pvo = 0 ;
		lastcnt = lastcs.cnt ;
		return ;
	}
	else if(lastcnt!=lastcs.cnt) {
		ltmp = lvma ;
		stmp = svma ;
		lastcnt = lastcs.cnt ;
	}

	lvma = ( ltmp * (ln-1) + lastcs.volume ) / double(ln) ;
	svma = ( stmp * (sn-1) + lastcs.volume ) / double(sn) ;
	pvo = 100.0 * ( svma - lvma ) / lvma ;
}

double PVOTransform::getPVO() {
	return pvo ;
}
