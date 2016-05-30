#pragma once

#include <string>
#include <vector>
#include "boost/date_time/posix_time/posix_time_types.hpp"

class StatLogger {
	struct LogRecord {
		boost::posix_time::ptime time ;
		double price ;
		double prediction ;
		double actual ;
	} ;
	boost::posix_time::time_duration duration ;
	std::vector<LogRecord> log ;

	typedef std::vector<LogRecord>::const_iterator LogIt ;


	// statistics
	double correlation, avgerr, sgnpct;
	double correlation_, avgerr_, sgnpct_ ;	// filtered
	double thresh ;	// abs(prediction) has to be bigger than this to take position when simulating
//	double lyield, lavg, lvar, lsyield, lsavg, lsvar ;

	void updateStats() ;

	static double computeSgnPct(const std::vector<LogRecord>& vec) ;
	static double computeAvgErr(const std::vector<LogRecord>& vec) ;
	static double computeCorrelation(const std::vector<LogRecord>& vec) ;
	void computeYields() ;
	static double computeAvg(const std::vector<double> &vec);
	static double computeVar(const std::vector<double> &vec, double avg);

public:
	StatLogger(const boost::posix_time::time_duration &_duration);
	void update(const boost::posix_time::ptime& time, double prediction, double actual, double price) ;

	std::wstring getDisplayText() ;
	std::wstring getRowText() ;
	std::wstring getStockRow() ;
	static std::wstring getFirstRow() ;
} ;