#pragma once

// a value synchronized to a candlestick series

struct SyncValue {
	unsigned long cscnt ;	// candlestick identifier
	double value ;
} ;