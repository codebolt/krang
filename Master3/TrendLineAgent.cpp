#include "TrendLineAgent.h"
#include "CandlestickSeries.h"

void TrendLineAgent::update()
{
	spv.update() ;

	const std::vector<SyncValue>& vec = spv.getVector() ;

	if(vec.size() < 5) return ;

	std::vector<SyncValue>::const_iterator pit = vec.end()-1 ;
	std::vector<SyncValue>::const_iterator tit = vec.end()-2 ;

	if(pit->cscnt == psrc->back().cnt) pit = pit-2 ;

	if(pit->value < tit->value) {
		// switch
		std::vector<SyncValue>::const_iterator tmp = pit ;
		pit = tit ;
		tit = tmp ;
	}

	signal = test( *pit, *(pit-2), false ) ;
	if(signal == 0.0) signal = test( *tit, *(tit-2), true ) ;
}

double TrendLineAgent::test(const SyncValue &sv1, const SyncValue &sv2, bool support)
{
	const unsigned LB = 5 ;
	const Candlestick& last = psrc->back() ;

//	SIM_ASSERT((sv1.cscnt > sv2.cscnt) && (last.cnt > sv1.cscnt)) ;

	double a = (sv1.value - sv2.value) / double(sv1.cscnt - sv2.cscnt) ;
	double target = sv1.value + double(last.cnt - sv1.cscnt) * a ;

	double prox = proximity(last.close, target);

	if(prox > 0.0) {
		// check that it's not already breached
		const std::vector<Candlestick>::const_reverse_iterator rit = psrc->getVec().rbegin() ;

		for(unsigned i=0; i<LB; i++) {
			if(support && (rit->high < target*(1.0-pthresh))) return 0.0;
			if((!support) && (rit->low > target*(1.0+pthresh))) return 0.0;
		}

		return prox * ( support ? 1.0 : -1.0 ) ;
	}

	return 0.0 ;
}

double TrendLineAgent::proximity(double price, double target)
{
	double diff = abs(price - target) ;
	double lim = target * pthresh ;

	if( diff < lim ) return 1.0 - diff / lim ;

	return 0.0 ;
}
