#include "SupportResistanceTest.h"
#include "CandlestickSeries.h"
#include "Util.h"

double SupportResistanceTest::srTest(const CandlestickSeries *pcss, const std::vector<SyncValue> &svvec, double thresh) {
	const unsigned lookback = 8 ;	// look-behind when determining past trend

	const std::vector<Candlestick> &csvec = pcss->getVec() ;

	if( svvec.size() < (lookback+1) ) return 0.0 ;

	std::vector<Candlestick>::const_reverse_iterator csit = csvec.rbegin() ;
	std::vector<SyncValue>::const_reverse_iterator svit = svvec.rbegin() ;

	double p = csit->close ;
	double ma = svit->value ;

	double a = ma*(1.0-thresh) ;
	double b = ma*(1.0+thresh) ;

	if( (p < a) || (p > b) ) {
		// too far away from potential support/resistance
		return 0.0 ;
	}

	double val = 1.0 - 2.0 * abs( (p - a) / (b - a) - 0.5 ) ;

	int trend = 0 ;
	unsigned cnt = 0 ;

	// check if we are coming out of a definite uptrend or downtrend
	for(unsigned i=0;i<lookback;i++) {
		if( MIN(csit->open, csit->close) > svit->value ) {
			if( trend == -1 ) return 0.0 ;
			trend = 1 ;
		}
		else if( MAX(csit->open, csit->close) < svit->value ) {
			if( trend == 1 ) return 0.0 ;
			trend = -1 ;
		}
		else cnt ++ ;
		csit++; 
		svit++;
	}

	return trend * val * (1.0 - cnt / (double) lookback)  ;
}

