#pragma once

class CandlestickSeries ;

class PVOTransform {
	const CandlestickSeries *psrc ;
	double pvo ;
	
	double lvma, svma ;
	const unsigned ln, sn;

	double ltmp, stmp ;
	unsigned lastcnt ;

public :
	PVOTransform(const CandlestickSeries* _psrc, unsigned lint, unsigned sint) : psrc(_psrc), ln(lint), sn(sint), lastcnt(0) { }

	void update() ;
	double getPVO() ;
} ;