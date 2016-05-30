#include "MATrendAgent.h"
#include "SimException.h"

MATrendAgent::MATrendAgent(const CandlestickSeries* _psrc, unsigned longInt, unsigned shortInt)
: Agent(), matc(_psrc, longInt, 1+(longInt+shortInt)/2, shortInt), psrc(_psrc) {
	using namespace boost::posix_time ;
}

void MATrendAgent::update() {
	matc.update() ;
	signal = matc.trend() ;
}