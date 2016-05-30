#pragma once

#include <vector>

#include "SyncValue.h"

class DivergenceTest {
	typedef std::vector<SyncValue> SvVec ;
	static bool divergence(const SyncValue& sv11, const SyncValue& sv12, const SyncValue& sv21, const SyncValue& sv22, unsigned cnt) ;
	static SvVec::const_reverse_iterator first(SvVec::const_reverse_iterator& rit, bool peak) ;

public:
	// checks two swing point vectors.. cnt=last cs cnt
	static double test(const SvVec& spv1, const SvVec& spv2, unsigned cscnt, bool peak) ;
};