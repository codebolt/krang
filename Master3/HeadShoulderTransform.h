#pragma once

#include "PlotOverlayTransform.h"
#include "SyncValue.h"

#include <vector>
#include <deque>

class SwingPointVector ;
struct Candlestick ;

struct HeadShoulderPattern {
	static const unsigned PATTERN_SIZE = 7 ;
	SyncValue sv[PATTERN_SIZE] ;
} ;

class HeadShoulderTransform : public PlotOverlayTransform {
	std::vector<HeadShoulderPattern> patterns ;
	std::deque<HeadShoulderPattern> candidates ;

	const SwingPointVector &spvec ;	// the threshold should be pretty tight here (3-4 pct on the daily?)
	// any pattern with a smaller distance from right shoulder to neckline will not be found

	unsigned prevspcnt ;
	unsigned prevcscnt ;

	unsigned matches ;	// count the number of currently matched patterns

	void findPatterns() ;

	// test if a pattern is a candidate (store if so)
	inline void testPattern(const SyncValue& p0, const SyncValue& t0, const SyncValue& p1, const SyncValue& t1,
						    const SyncValue& p2, const SyncValue& t2, const SyncValue& p3) ;

	// test if the candlestick breaks the neckline of the candidate pattern
	bool necklineBreak(const HeadShoulderPattern& hsp, const Candlestick& cs, double prevhigh) ;

public:
	HeadShoulderTransform(const CandlestickSeries *_psrc, const SwingPointVector& _spvec)
		: PlotOverlayTransform(_psrc), spvec(_spvec), prevspcnt(0), prevcscnt(0) { }

	void update() ;
	void draw(HDC hDC, double hhigh, double llow, unsigned h, unsigned w, unsigned cnt0, unsigned cnt1) const ;
	unsigned getMatches() const { return matches; }
} ;