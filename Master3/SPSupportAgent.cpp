#include "SPSupportAgent.h"
#include "CandlestickSeries.h"
#include "Util.h"

SPSupportAgent::SPSupportAgent(const CandlestickSeries *_psrc, double spthresh, double _lthresh, unsigned _lookback) : 
Agent(), spvec(_psrc, spthresh), lthresh(_lthresh),  matc(_psrc, 20, 10, 5), pcss(_psrc), lookback(_lookback) {
	transforms.push_back( &spvec ) ;
	// don't want mas to be drawn
}

void SPSupportAgent::update() {
	const unsigned LB = 10 ;
	matc.update() ;
	spvec.update() ;

	double trend = matc.trend() ;

	if(abs(trend) < 1.0) {
		signal = 0.0 ;
		return ;
	}

	double price = pcss->back().close ;


	// check the levels
	if(spvec.getVector().size() <= 1) return ;

	const std::vector<SyncValue>& vec = spvec.getVector() ;

	std::vector<SyncValue>::const_reverse_iterator rit = vec.rbegin() ;

	unsigned u = 0 ;
	double sum = 0.0 ;

	while((++rit != vec.rend()) && (u++ < LB)) {
		sum += proximity(price, rit->value) ;
	}

	signal = -1.0 * trend * MIN(1.0, sum) ;
}

double SPSupportAgent::proximity(double price, double level) {
	double delta = level * lthresh ;
	double diff = abs(price - level) ;

	if( diff < delta ) {
		return 1.0 - diff / delta ; 
	}
	else return 0.0 ;
}

