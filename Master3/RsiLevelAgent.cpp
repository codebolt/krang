#include "RsiLevelAgent.h"

void RsiLevelAgent::update()
{
	transform.update() ;

	if(transform.empty()) return ;

	double rsi = transform.latest() ;

	if(rsi > 70.0) {
		signal = -0.5 + 0.5 * (70.0 - rsi) / 30.0 ;
	}
	else if(rsi < 30.0) {
		signal = 0.5 + 0.5 * (30.0 - rsi) / 30.0 ;
	}
	else {
		signal = 0.0 ;
	}
}
