#include "DivergenceTest.h"
#include "Util.h"

double DivergenceTest::test(const SvVec &spv1, const SvVec &spv2, unsigned cscnt, bool peak) {
	SvVec::const_reverse_iterator rit1 = spv1.rbegin() ;
	SvVec::const_reverse_iterator rit2 = spv2.rbegin() ;

	const unsigned N1 = spv1.size() ;
	const unsigned N2 = spv2.size() ;

	if( MIN(N1,N2) < 5 ) return 0.0 ;

	// go to first peak?peak:trough 
	rit1 = first(rit1, peak) ;
	rit2 = first(rit2, peak) ;

	bool test = peak ? 
		divergence(*(rit2+2), *rit2, *(rit1+2), *rit1, cscnt) :	// rsi up, cs down
		divergence(*(rit1+2), *rit1, *(rit2+2), *rit2, cscnt) ; // cs down, rsi up

	if(test) return peak ? 1.0 : -1.0 ;


	return 0.0 ;
}

bool DivergenceTest::divergence(const SyncValue &sv11, const SyncValue &sv12, const SyncValue &sv21, const SyncValue &sv22, unsigned int cnt) {
	// test if sv1 points up and sv2 points down

	// check symmetry
	if( (sv11.cscnt < sv22.cscnt) && (sv21.cscnt < sv12.cscnt) && (MIN(sv22.cscnt, sv12.cscnt)>(int(cnt)-20)) ) {
		// check div
		return (sv11.value < sv12.value) && (sv21.value > sv22.value) ;
	}

	return false;
}

DivergenceTest::SvVec::const_reverse_iterator DivergenceTest::first(DivergenceTest::SvVec::const_reverse_iterator &rit, bool peak) {
	// go to first swing point
	if(rit->value > (rit+1)->value) {
		return peak ? (rit+2) : (rit+1) ;
	}
	else {
		return peak ? (rit+1) : (rit+2) ;
	}

}