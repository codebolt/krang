#pragma once

#include "Candlestick.h"
#include "Trade.h"

class Agent ;

#include "SimException.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <vector>

class CandlestickSeries {
	static unsigned idgen;

	const unsigned id ;
	std::vector<Candlestick> series ;
	std::vector<Agent*> agents ;

	boost::posix_time::time_period current ;
	boost::posix_time::time_duration duration ;

	void addCandle(const Candlestick& cs) { series.push_back(cs); }
	void updateAgents(const boost::posix_time::ptime& pt) ;

public:
	typedef std::vector<Candlestick>::const_iterator ConstIterator ;
	typedef std::vector<Candlestick> CsVec ;
	typedef std::vector<Agent*> AgentVec ;

	CandlestickSeries(boost::posix_time::time_period initial) ;

	void nextTrade();

	ConstIterator begin() const { return series.begin(); }
	ConstIterator end() const { return series.end(); }
	ConstIterator iterator(int from) const { return series.begin()+from; }

	const Candlestick& at(int i) const { return series[i] ; }
	const Candlestick& back() const { return series.back(); }
	const Candlestick& nlast(unsigned n) const { return *(series.rbegin()+n); }
	unsigned size() const { return series.size(); }	// can be optimized
	bool empty() const { return series.empty(); }

	const CsVec& getVec() const { return series; }

	bool operator==(const CandlestickSeries& css) const { return id==css.id; }
	std::wstring toString() const ;

	const boost::posix_time::time_duration& getDuration() const { return duration; }

	void addAgent(Agent* agent) { agents.push_back(agent); }
	const AgentVec& getAgents() const { return agents; }
};

