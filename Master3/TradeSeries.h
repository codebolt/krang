#pragma once

#include "Trade.h"
#include "Ticker.h"
#include <vector>
#include "boost/date_time/gregorian/gregorian_types.hpp"
class TradeSeries {
	std::vector< Trade > trades ;

public:
	typedef std::vector< Trade >::const_iterator ConstIterator;

	TradeSeries() { } ;

	unsigned read(Ticker ticker, boost::gregorian::date_period dp, const std::wstring& dataDir) ;	// returns number of trades read
	void filter() ;	// remove trash/noise from data

	ConstIterator begin() const { return trades.begin(); }
	ConstIterator end() const { return trades.end(); }

	unsigned size() const { return trades.size(); }
} ;