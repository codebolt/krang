#pragma once

#include "boost/date_time/posix_time/posix_time_types.hpp"

struct Candlestick {
	unsigned long cnt ;	// candlestick count

	unsigned long volume ;
	double close ;
	double open ;
	double high ;
	double low ;
	boost::posix_time::time_period tp ;
} ;