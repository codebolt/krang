#include "HeadShoulderAgent.h"

HeadShoulderAgent::HeadShoulderAgent(const CandlestickSeries *_psrc, double spthreshold) : Agent(), spvec(_psrc,spthreshold), hst(_psrc,spvec)
{
	this->transforms.push_back( &spvec ) ;
	this->transforms.push_back( &hst ) ;
}

void HeadShoulderAgent::update()
{
	spvec.update() ;
	hst.update() ;
	signal = hst.getMatches()>0 ? 100.0 : 0.0 ;
}
