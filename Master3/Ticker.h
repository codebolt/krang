#pragma once

#include <string>

enum Ticker {
	DNBNOR, 
	FRO, 
	GOGL, 
	JIN, 
	NAS, 
	NHY, 
	ORK, 
	STB, 
	YAR,
	INVALID
} ;

std::wstring tickerToString( Ticker t ) ;
Ticker tickerFromString( const std::wstring& str ) ;
