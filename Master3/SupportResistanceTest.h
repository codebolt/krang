#pragma once

#include "SyncValue.h"

class CandlestickSeries ;

#include <vector>

class SupportResistanceTest {
public:
	enum { NONE, SUPPORT, RESISTANCE, UPTREND, DOWNTREND } ;
	static double trendTest(const CandlestickSeries *pcss, const std::vector<SyncValue> &svvec, double thresh) ;
	static double srTest(const CandlestickSeries *pcss, const std::vector<SyncValue> &svvec, double thresh) ;
} ;