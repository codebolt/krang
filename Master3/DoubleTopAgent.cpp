#include "DoubleTopAgent.h"

void DoubleTopAgent::update() 
{
	spv.update() ;
	signal = 0.0 ;

	const std::vector<SyncValue>& vec = spv.getVector() ;

	if( vec.size() < 7 ) return ;

	// must be coming out of an uptrend trend, test the previous peaks
	std::vector<SyncValue>::const_iterator it = vec.end() - 2;

	// check that this is a peak
	if(it->value < (it+1)->value) return ;

	double currpeak = it->value ;
	double prevpeak = (it-2)->value ;

	if( (currpeak > prevpeak*(1.0-pthresh)) && (currpeak < prevpeak*(1.0+pthresh)) ) {
		for(int i=2;i<=4;i+=2)
			if( (it-i-2)->value > (it-i)->value ) return ;

		for(int i=1;i<=3;i+=2)
			if( (it-i-2)->value > (it-i)->value ) return ;

		signal = -1.0 ;
	}
}
