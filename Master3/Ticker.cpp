#include "Ticker.h"
#include "SimException.h"

std::wstring tickerToString( Ticker t ) {
	switch(t) {
	case DNBNOR: return L"DNBNOR"; 
	case FRO: return L"FRO"; 
	case GOGL: return L"GOGL"; 
	case JIN: return L"JIN"; 
	case NAS: return L"NAS"; 
	case NHY: return L"NHY"; 
	case ORK: return L"ORK"; 
	case STB: return L"STB";
	case YAR: return L"YAR";
	}

	throw SIM_EX( L"Invalid ticker value." ) ;
}

Ticker tickerFromString( const std::wstring& str ) {
	if(!str.compare(L"DNBNOR")) return DNBNOR ;
	else if(!str.compare(L"FRO")) return FRO ;
	else if(!str.compare(L"GOGL")) return GOGL ;
	else if(!str.compare(L"JIN")) return JIN ;
	else if(!str.compare(L"NAS")) return NAS ;
	else if(!str.compare(L"NHY")) return NHY ;
	else if(!str.compare(L"ORK")) return ORK ;
	else if(!str.compare(L"STB")) return STB ;
	else if(!str.compare(L"YAR")) return YAR ;

	throw SIM_EX( L"Invalid ticker string." ) ;
}