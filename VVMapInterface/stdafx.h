#ifndef _WIN32_WINNT
#define  _WIN32_WINNT  0x0500
#endif

#include <assert.h>
#include <windows.h>
#include <atlbase.h>
#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>

#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iterator>


#undef min
#undef max

template <class T> T sq(T x) { return x*x; }
template <class T> T sqr(T x) { return x*x; }
template <class T> T cube(T x) { return x*x*x; }
template <class T> void zero(T& x) { ::memset ( &x, 0, sizeof(T) ); }
template <class T> T sign(T x) { return x > 0 ? +1 : x > 0 ? -1 : 0; }

#define ALL(c) (c).begin(), (c).end()

typedef float MyFloat;

template <class T> T * GetPtr(std::vector<T>& v) { return v.size() ? &v[0] : 0; }
template <class T> const T * GetPtr(const std::vector<T>& v) { return v.size() ? &v[0] : 0; }

typedef std::vector<int> int_vect;

#include "sys.h"

inline void MyTrace (const std::wstring& msg)
{
#ifdef _DEBUG
	::OutputDebugStringW ( msg.c_str() );
#endif
}

inline void MyTrace (const std::string& msg)
{
#ifdef _DEBUG
	MyTrace(sys::WIDE_str(msg.c_str()) );
#endif
}