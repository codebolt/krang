#include "MATrendClassifier.h"

void MATrendClassifier::update() {
	lma.update() ;
	sma.update() ;
	mma.update() ;
}

double MATrendClassifier::trend() {
	if(lma.empty() || sma.empty() || mma.empty()) return 0.0 ;

	double l = lma.last();
	double m = mma.last();
	double s = sma.last();

	if( l > m ) {
		if( m > s ) {
			return -1.0 ;
		}
		else if(l > s) {
			return -0.5 ;
		}
	}
	else {
		if( m < s ) {
			return +1.0 ;
		}
		else if(l < s) {
			return +0.5 ;
		}
	}

	return 0.0 ;
}