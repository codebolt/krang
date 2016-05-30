#include "VolumeAgent.h"
#include "CandlestickSeries.h"
#include "Util.h"

void VolumeAgent::update()
{
	pvo.update() ;
	matc.update() ;

	if(matc.trend() == 0.0) return ;

	if( (matc.trend() > 0.0) && (pvo.getPVO() > 0.0) ) {
		signal = matc.trend() * MIN(pvo.getPVO(), 50.0) / 50.0 ;
	}
	else if((matc.trend() < 0.0) && (pvo.getPVO() < 0.0)) {
		signal = -1.0 * matc.trend() * MAX(pvo.getPVO(), -50.0) / 50.0 ;
	}
	else {
		signal = 0.0 ;
	}
}
