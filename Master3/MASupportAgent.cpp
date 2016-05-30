#include "MASupportAgent.h"
#include "SupportResistanceTest.h"

MASupportAgent::MASupportAgent(const CandlestickSeries* _psrc, unsigned intervals, double _threshold)
: Agent(), transform(_psrc, intervals), psrc(_psrc), threshold(_threshold) {
	using namespace boost::posix_time ;
	transforms.push_back(&transform) ;
}

void MASupportAgent::update() {
	using namespace std ;
	transform.update() ;
	signal = SupportResistanceTest::srTest(psrc, transform.getVector(), threshold) ;
}