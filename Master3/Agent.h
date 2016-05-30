#pragma once

#include <vector>

#include "AgentSignal.h"
#include "Plot.h"
#include "boost/date_time/posix_time/posix_time.hpp"

class Transform ;

class Agent : public Plot {
protected:
	struct SignalRecord {
		boost::posix_time::ptime pt ;
		double signal ;
	} ;

	std::vector<SignalRecord> signalHistory ;
	std::vector<Transform*> transforms ;
	std::wstring labels[PLOT_STEPS] ;

	double signal ;
	virtual void update() = 0;

	void drawPlot();
	void setText(unsigned offset);

public:
	typedef std::vector<Transform*> TransformVec ;

	Agent() : Plot(false), signal(0.0) { } 
	const TransformVec& getTransforms() const { return transforms; }
	double getSignal() const { return signal; } 

	void update(const boost::posix_time::ptime &time) ;
	bool active() { return !signalHistory.empty(); }
	virtual std::wstring toString() = 0;
} ;