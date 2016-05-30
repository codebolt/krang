#include "RsiDivAgent.h"
#include "DivergenceTest.h"
#include "CandlestickSeries.h"
#include "SimException.h"
#include "Util.h"

RsiDivAgent::RsiDivAgent(const CandlestickSeries* _psrc, unsigned intervals, double csthresh) 
		: Agent(), psrc(_psrc), transform(_psrc, intervals), csspv(_psrc, csthresh)
{
//	rsispv.initsrc(& transform.getVec()) ;
//	transforms.push_back( &transform ) ;
	transforms.push_back( &csspv ) ;
}

void RsiDivAgent::update()
{
	transform.update() ;
	csspv.update() ;

	if(transform.empty() || csspv.empty()) return ;

	const std::vector<SyncValue>& spvec = csspv.getVector() ;

	if(spvec.size() < 5) return ;

	bool peaks = transform.latest() > 50.0 ;

	std::vector<SyncValue>::const_reverse_iterator rit = spvec.rbegin();

	unsigned offset = ((rit->cscnt+5)>=psrc->back().cnt) ? 0 : 2 ;

	if(peaks) {
		rit = (rit->value < (rit+1)->value) ? rit+1 : rit+offset ;
	}
	else {
		rit = (rit->value < (rit+1)->value) ? rit+offset : rit+1 ;
	}

	std::vector<SyncValue>::const_reference sp1 = *(rit+2), sp2 = *rit ;

	double rsi1 = getRSI(sp1.cscnt, peaks) ;
	double rsi2 = getRSI(sp2.cscnt, peaks) ;

	signal = 0.0 ;

	if( (rsi1<0.0) || (rsi2<0.0) ) return ;		// some kind of error

	if(peaks) {
		if((rsi1 < rsi2) && (sp1.value > sp2.value)) signal = -1.0 ;
	}
	else {
		if((rsi2 > rsi1) && (sp1.value < sp2.value)) signal = 1.0 ;
	}

/*
	if(!transform.empty()) {
		updateSPV() ;
		if((!csspv.getVector().empty()) && !rsispv.getVector().empty())
			signal = DivergenceTest::test(csspv.getVector(), rsispv.getVector(), psrc->back().cnt, transform.latest()>50.0 ) ;
	}
*/
}

double RsiDivAgent::getRSI(unsigned int cscnt, bool peak) {
	const std::vector<SyncValue>& vec = transform.getVec() ;
	unsigned offset = vec.front().cscnt ;

	if(cscnt <= offset) return -1.0 ;

	std::vector<SyncValue>::const_iterator it = vec.begin() + (cscnt-offset) ;
	SIM_ASSERT( it->cscnt == cscnt ) ;

	if(vec.end() == (it+1)) return it->value ;

	double value ;
	if(peak) value = MAX( it->value, MAX((it+1)->value, (it-1)->value) ) ;
	else value = MIN( it->value, MIN((it+1)->value, (it-1)->value) ) ;
	return value ;
}



/*
void RsiDivAgent::updateSPV() {

	rsispv.update() ;

	// update the rsi spv

}
*/