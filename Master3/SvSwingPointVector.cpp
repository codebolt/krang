#include "SvSwingPointVector.h"
#include "SimException.h"

#include <cassert>

void SvSwingPointVector::update() {
	SIM_ASSERT(!!psrc) ;

	if(spvec.empty()) initial() ;
	else {
		const SyncValue& cs = psrc->back() ;
		SyncValue& sp = spvec.back();

		switch(dir) {
		case UP:
			if( cs.value>sp.value ) {
				sp.cscnt = cs.cscnt ;
				sp.value = cs.value ;
			}
			else if( cs.value<sp.value*(1.0-threshold) && (sp.cscnt != cs.cscnt) ) {
				SyncValue tmp = { cs.cscnt, cs.value } ;
				spvec.push_back(tmp) ;
				dir = DOWN ;
			}
			break ;
		case DOWN:
			if( cs.value<sp.value ) {
				sp.cscnt = cs.cscnt ;
				sp.value = cs.value ;
			}
			else if( cs.value>sp.value*(1.0+threshold) && (sp.cscnt != cs.cscnt) ) {
				spvec.push_back(cs) ;
				dir = UP ;
			}
			break;
		default:
			throw SIM_EX(L"Krang is angry");	// something bad happened
		}
	}
}

void SvSwingPointVector::initial() {
	using namespace std ;

	// spvec is empty, look for the first swing point
	vector<SyncValue>::const_iterator csit = psrc->begin() ;

	if(psrc->empty()) return ;

	const SyncValue* plow = &*csit;
	const SyncValue* phigh = &*csit ;

	while((++csit) != psrc->end()) {
		if(plow->value > csit->value) plow = &*csit ;
		if(phigh->value < csit->value) phigh = &*csit ;
	}

	if(phigh->value > plow->value*(1.0+threshold)) {
		// add them in chronological order
		if(phigh->cscnt > plow->cscnt) {
			spvec.push_back(*plow) ;
			spvec.push_back(*phigh) ;
			dir = UP ;
		}
		else {
			spvec.push_back(*phigh) ;
			spvec.push_back(*plow) ;
			dir = DOWN ;
		}
	}
}
