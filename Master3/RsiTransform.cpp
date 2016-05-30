#include "RsiTransform.h"
#include "CandlestickSeries.h"

void RsiTransform::update() {
	using namespace std; 

	const Candlestick& lastcs = psrc->back() ;
	bool add ;	// add new to vec?

	if(this->vec.empty()) {
		if(psrc->size() > intervals) add=true ;
		else return ;
	}
	else if(this->vec.back().cscnt == psrc->back().cnt) add = false ;
	else {
		SIM_ASSERT( this->vec.back().cscnt == (psrc->back().cnt-1) ) ;
		add = true ;
	}

	if(add) {
		SyncValue next = { lastcs.cnt, calc() } ;
		vec.push_back(next) ;
	}
	else {
		vec.back().value = calc() ;
	}
}

double RsiTransform::calc() {
	double avgup = 0.0 ;
	double avgdn = 0.0 ;
	unsigned nup = 0 ;
	unsigned ndown = 0 ;

	std::vector<Candlestick>::const_reverse_iterator rit = psrc->getVec().rbegin() ;

	unsigned i = 0 ;
	while(i++ < intervals) {
		double next = rit->close ;
		++ rit ;

		if( rit->close < next ) {
			nup ++ ;
			avgup += next - rit->close ;
		}
		else if( rit->close > next ) {
			ndown ++ ; 
			avgdn += rit->close - next ;
		}
	}

	if(!nup) return 0.0 ;
	else if(!ndown) return 100.0 ;

	avgup /= nup ;
	avgdn /= ndown ;

	return 100.0 - 100.0/(1.0 + avgup/avgdn) ;
}
