#include "TradeSeries.h"
#include "WinInclude.h"
#include "SimException.h"
#include "MainDlg.h"
#include "boost/date_time/posix_time/posix_time.hpp"

unsigned TradeSeries::read(Ticker ticker, boost::gregorian::date_period dp, const std::wstring& dataDir) {
	using namespace std;

	struct RawTrade {
		time_t time ;
		double price ;
		unsigned long volume ;
	} ;

	wstring tickerString = tickerToString(ticker);
	wstring filename = tickerString + L".dat";
	wstring fullPath = dataDir + tickerString + L".dat";

	MainDlg::log(L"Reading trade data from: " + filename + L"...");

	HANDLE hFile = CreateFile(fullPath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE) {
		throw SIM_EX( L"Unable to open file: " + fullPath ) ;
	}

	const size_t count = GetFileSize(hFile, NULL) / sizeof(RawTrade);

	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	if(hMap == NULL) {
		CloseHandle(hFile);
		throw SIM_EX( L"Unable to create map of file: " + filename ) ;
	}

	LPVOID pMap = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

	if(pMap == NULL) {
		CloseHandle(hMap);
		CloseHandle(hFile);
		throw SIM_EX( L"Unable to create view of file map: " + filename ) ;
	}

	// start reading the file

	// identify the first and last index we are interested in
	trades.clear();
	trades.reserve(count);	// sometimes we won't need all of it, but go ahead anyway

	const RawTrade *tcs = (RawTrade*) pMap;

	unsigned cnt = 0;
	for(unsigned i=0;i<count;tcs++,i++) {
		boost::posix_time::ptime t = boost::posix_time::from_time_t( tcs->time );
		boost::gregorian::date d = t.date();

		if(dp.contains(d)) {
			Trade trade = { cnt++, t, tcs->price, tcs->volume } ;
			trades.push_back( trade );
		}
		else if(dp.is_before(d)) break ;	// no more trades
	}

	UnmapViewOfFile(pMap);
	CloseHandle(hMap);
	CloseHandle(hFile);

	MainDlg::log(L"Finished! Read " + x2s(count) + L" trades.\r\n");

	return trades.size() ;
}

void TradeSeries::filter() {
	MainDlg::log(L"Filtering trades... ");

	const double HI_THRESH = 0.04 ;	// 20% change 
//	const double LOW_THRESH = 0.100 ;
//	const int MAX_COUNT = 10;

	std::vector< Trade > tmpvec ;
	tmpvec.reserve( trades.size() ) ;

	std::vector<Trade>::iterator it=trades.begin();

	double p0=it->price;
	int tot_cnt = 0; 

	for(; it!=trades.end(); ++it) {
		if( (it->time.time_of_day().hours() < 9) || (it->time.time_of_day().hours() > 17) ) {
			// irregular time messes up the candlestick generation
			tot_cnt ++;
		}
		else {
			if(abs(it->price-p0)/p0 > HI_THRESH) {
				double sgn = (it->price<p0)?-1.0:+1.0 ;
				it->price = p0*(1.0 + sgn*0.01) ;
			}
			tmpvec.push_back( *it ) ;
			p0 = it->price ;
		}
	}

	trades = tmpvec ;

	MainDlg::log(L"Finished! Filtered out " + x2s(tot_cnt) + L" trades.\r\n");
}
