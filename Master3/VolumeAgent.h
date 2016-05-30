#pragma once

#include "Agent.h"
#include "PVOTransform.h"
#include "MATrendClassifier.h"

class CandlestickSeries ;

class VolumeAgent : public Agent {
	PVOTransform pvo ;
	MATrendClassifier matc ;

	void update() ;

public:
	VolumeAgent(const CandlestickSeries *psrc, unsigned lint, unsigned sint) : Agent(), pvo(psrc, lint, sint), matc(psrc, lint, (lint-sint)/2, sint) { }
	std::wstring toString() { return L"VolumeAgent"; }
} ;