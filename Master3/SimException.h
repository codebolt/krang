#pragma once

#define _WIDEN(x) L ## x
#define _WIDEN2(x) _WIDEN(x)

#define SIM_EX(str) SimException(str, _WIDEN2( __FILE__ ), __LINE__)

#define QUOTE(x) L#x
#define SIM_ASSERT(test) if(!(test)) throw SIM_EX(std::wstring(L"Assertion failed: ") + QUOTE(test))

#include <sstream>

class SimException {
	std::wstring str ;
	std::wstring file ;
	unsigned line ;

public:
	SimException(const std::wstring& _str, const std::wstring& _file, unsigned _line) : str(_str), file(_file), line(_line) {};
	void log();
} ;

template<class T> std::wstring x2s(T x) {
	std::wstringstream wss;
	wss << x ;
	return wss.str();
}

