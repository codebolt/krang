#pragma once

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#include <string>

template<class T> inline T MIN(const T& a, const T& b) { return (a<b) ? a : b ; }
template<class T> inline T MAX(const T& a, const T& b) { return (a>b) ? a : b ; }

bool fileExists(const std::wstring& path) ;
