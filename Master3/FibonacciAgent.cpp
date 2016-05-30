#include "FibonacciAgent.h"
#include "Util.h"
#include "CandlestickSeries.h"

void FibonacciAgent::update() {
	spv.update();
	matc.update();

	const std::vector<SyncValue>& vec = spv.getVector();

	if(vec.size() < 3) return ;

	std::vector<SyncValue>::const_reverse_iterator rit = vec.rbegin() ;

	bool support = rit->value > (rit+1)->value ;
	double trend = matc.trend() ;

	if(trend == 0.0) { signal=0.0; return; }

	double price = psrc->back().close ;
	double val;
	
	if( ((support) && (trend<0.0)) || ((!support) && (trend>0.0)) ) {
		val = fib(price, rit->value, rit->value - (rit+1)->value) ;
	}
	else {
		val = fib(price, (rit+1)->value, (rit+1)->value - (rit+2)->value) ; 
	}

	signal = abs(trend) * val ;
}

double FibonacciAgent::proximity(double price, double level) {
	double delta = level * pthresh ;
	double diff = abs(price - level) ;

	if( diff < delta ) {
		return 1.0 - diff / delta ; 
	}
	else return 0.0 ;
}

double FibonacciAgent::fib(double price, double pivot, double diff) {
	static double c[] = { 0.236, 0.382, 0.500, 0.618, 1.0 } ;
	static double w[] = { 0.5, 0.75, 0.75, 1.0, 0.75 } ;

	double sum = 0.0 ;

	for(int i=0;i<5;i++) sum += proximity(price, pivot - w[i]*c[i]*diff) ; 

	return ((diff>0.0) ? 1.0 : -1.0) * MIN(1.0, sum) ; 
}