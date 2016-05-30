#pragma once

#include "Agent.h"
#include "RsiTransform.h"
#include "SwingPointVector.h"
#include "MATrendClassifier.h"

class RsiDivAgent : public Agent {
private:
	RsiTransform transform ;
	SwingPointVector csspv ;
//	SvSwingPointVector rsispv ;

	double rsithresh ;

	const CandlestickSeries* psrc ;

//	void updateSPV() ;

	double getRSI(unsigned cscnt, bool peak) ;

public:
	RsiDivAgent(const CandlestickSeries* _psrc, unsigned intervals, double csthresh) ;

	void update() ;
	std::wstring toString() { return L"RsiDivAgent"; }
} ;