#pragma once

#include "boost/date_time/posix_time/posix_time_types.hpp"

struct Trade {
	unsigned cnt ;	// counter

	boost::posix_time::ptime time ;
	double price ;
	unsigned long volume ;
} ;

