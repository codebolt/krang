#include "StatLogger.h"
#include "SimException.h"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <cmath>

StatLogger::StatLogger(const boost::posix_time::time_duration &_duration) : duration(_duration)
{
	using namespace boost::posix_time ;
	log.reserve(1024) ;

	if(duration <= time_duration(1,0,0)) thresh = 0.01 ;
	else if(duration <= time_duration(24,0,0)) thresh = 0.02 ;
	else thresh = 0.04 ;
}

void StatLogger::update(const boost::posix_time::ptime &time, double prediction, double actual, double price)
{
	LogRecord lr = { time, price, prediction, actual } ;
	log.push_back(lr) ;
}

std::wstring StatLogger::getDisplayText()
{
	// text displayed in the gui
	return L"[" + boost::posix_time::to_simple_wstring(duration) + L"]...\r\n" ;
}

std::wstring StatLogger::getFirstRow()
{
	// first row in the log table
	return L"Node & Corr & AE & Sgn & Corr* & AE* & Sgn* \\\\ \r\n" ; 
}

std::wstring StatLogger::getRowText()
{
	std::wstringstream ss ;

	updateStats() ;
	ss.precision( 4 ) ;
	ss << L"[" << boost::posix_time::to_simple_wstring(duration) << L"] & "	// node name is duration
		<< correlation << L" & " << avgerr << L" & " << sgnpct << L" & " 
		<< correlation_ << L" & " << avgerr_ << L" & " << sgnpct_ << L" \\\\ "; 

	return ss.str() ; 
}

std::wstring StatLogger::getStockRow()
{
	// first row in the log table
	const unsigned N = log.size() ;
	double p0 = log.front().price ;
	double p1 = log.back().price ;
	double yield = (p1-p0)/p0 ;
	double avg = yield / static_cast<double>(N-1) ;
	double var = 0.0 ;

	// compute stock variance
	double prev = log.front().price ;
	for(LogIt it = log.begin()+1 ; it != log.end() ; it++) {
		double chg = (it->price - prev) / prev ;
		double diff = chg - avg ;
		var += diff*diff;
	}
	var /= N-1 ;

	std::wstringstream ss ;
	ss << L"Stock & 1.0 & 0.0 & 100.0 & " << yield << L" & " << avg << L" & " << sqrt(var) << L"& - & - & - \\\\ \r\n" ;

	return ss.str() ;
}

void StatLogger::updateStats()
{
	correlation = computeCorrelation(log);
	sgnpct = computeSgnPct(log);
	avgerr = computeAvgErr(log);

	double thresh=0.0 ;
	for(LogIt it=log.begin();it!=log.end();it++) {
		thresh += it->prediction*it->prediction ;
	}
	thresh = sqrt(thresh/double(log.size())) ;

	std::vector<LogRecord> filtered ;
	for(LogIt it=log.begin();it!=log.end();it++) {
		if( (it->prediction > thresh) || (it->prediction < -thresh) )
			filtered.push_back( *it ) ;
	}

	correlation_ = computeCorrelation(filtered);
	sgnpct_ = computeSgnPct(filtered);
	avgerr_ = computeAvgErr(filtered);

//	computeYields();
}

double StatLogger::computeCorrelation(const std::vector<LogRecord>& vec)
{
	double _corr = 0.0;
	double predMean, predVar, realMean, realVar ;
	size_t N = vec.size() ;
	SIM_ASSERT(N>0) ;

	predMean = predVar = realMean = realVar = 0.0 ;

	for(LogIt it = vec.begin() ; it != vec.end() ; it++) {
		predMean += it->prediction ;
		realMean += it->actual ;
	}

	predMean /= N ;
	realMean /= N ;

	for(LogIt it = vec.begin() ; it != vec.end(); it++) {
		double predDiff = it->prediction - predMean ;
		double realDiff = it->actual - realMean ;
		predVar += predDiff * predDiff ;
		realVar += realDiff * realDiff ;
		_corr += predDiff * realDiff ;
	}

	predVar /= N ;
	realVar /= N ;

	double predSD = sqrt(predVar) ;
	double realSD = sqrt(realVar) ;

	_corr /= N * predSD * realSD ;

	return _corr ;
}

double StatLogger::computeSgnPct(const std::vector<LogRecord>& vec)
{
	using namespace std; 

	unsigned cnt = 0;

	for(LogIt it = vec.begin() ; it != vec.end() ; it++) {
		// check for same sign on prediction and actual
		if( ((it->prediction < 0.0) && (it->actual < 0.0)) || ((it->prediction >= 0.0) && (it->actual >= 0.0)) ) 
			cnt ++ ;
	}

	return static_cast<double>(cnt) / static_cast<double>(vec.size()) ;
}

double StatLogger::computeAvgErr(const std::vector<LogRecord>& vec)
{
	double sum = 0.0 ;

	for(LogIt it = vec.begin() ; it != vec.end() ; it++) {
		sum += abs(it->prediction - it->actual) ;
	}

	return sum / double(vec.size()) ;
}

double StatLogger::computeAvg(const std::vector<double> &vec) {
	double sum = 0.0 ;
	for(std::vector<double>::const_iterator it = vec.begin(); it!=vec.end(); it++) {
		sum += *it ;
	}
	return sum / static_cast<double>( vec.size() ) ;
}

double StatLogger::computeVar(const std::vector<double> &vec, double avg) {
	double sum = 0.0 ;
	for(std::vector<double>::const_iterator it = vec.begin(); it!=vec.end(); it++) {
		double diff = avg - *it ; 
		sum += diff * diff ;
	}
	return sum / static_cast<double>( vec.size() ) ;
}



void StatLogger::computeYields()
{
/*	using namespace std;

	vector<double> lvec ;
	vector<double> lsvec ;

	const unsigned N = log.size();

	lvec.reserve(N-1);
	lsvec.reserve(N-1);

	lyield = lsyield = 1.0 ;

	double pred = 0.0 ;
	double prev = 0.0 ;
	for(LogIt it = log.begin()+1 ; it != log.end() ; it++) {
		double lchg = 0.0, lschg = 0.0 ;
		
		if(pred > thresh) {
			lschg = lchg = (it->price-prev)/prev ;
		}
		else if(pred < -thresh) {
			lschg = -1.0 * (it->price-prev)/prev ;
		}

		lyield *= 1.0 + lchg ;
		lsyield *= 1.0 + lschg ;
		lvec.push_back(lyield);
		lsvec.push_back(lsyield);

		pred = it->prediction ;
		prev = it->price ;
	}

	lavg = computeAvg(lvec);
	lsavg = computeAvg(lsvec);
	lvar = computeVar(lvec,lavg);
	lsvar = computeVar(lsvec,lsavg); */
}