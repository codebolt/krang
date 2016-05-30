#pragma once

class CandlestickSeries ;

class Transform {
protected:
	const CandlestickSeries *psrc ;

public:
	Transform(const CandlestickSeries *_psrc) : psrc(_psrc) {};
	virtual void update()=0;
} ;