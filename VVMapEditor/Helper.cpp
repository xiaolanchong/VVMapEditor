// Helper.cpp: implementation of the Helper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Helper.h"
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace Helper  
{
/*
#ifdef _UNICODE

std::wstring	Convert(CString str)
{
	return std::wstring( str );
}

CString			Convert(const std::wstring& str)
{
	return CString ( str.c_str() ); 
}

#else

std::wstring	Convert(CString str)
{
	std::vector<wchar_t> buf;
	int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCTSTR)str, -1,  &buf[0], 0);
	buf.resize(size);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCTSTR)str, -1,  &buf[0], size);
	return std::wstring( &buf[0] );	
}

CString			Convert(const std::wstring& str)
{
	std::vector<char> buf;
	int size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str.c_str(), -1,  &buf[0], 0, 0, 0);
	buf.resize(size);
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str.c_str(), -1,  &buf[0], size, 0, 0);
	return CString( &buf[0] );	
}

#endif

std::wstring	Convert(const std::string& str)
{
	std::vector<wchar_t> buf;
	int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1,  &buf[0], 0);
	buf.resize(size);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1,  &buf[0], size);
	return std::wstring( &buf[0] );	
}*/

std::string		ConvertA(LPCWSTR szStr)
{
	std::vector<char> buf;
	int size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szStr, -1,  &buf[0], 0, 0, 0);
	buf.resize(size);
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, szStr, -1,  &buf[0], size, 0, 0);
	return std::string	( &buf[0] );
}

std::wstring	ConvertW(LPCWSTR szStr)
{
	return std::wstring(szStr);
}

std::string		ConvertA(LPCSTR  szStr)
{
	return std::string(szStr);
}

std::wstring	ConvertW(LPCSTR  szStr)
{
	std::vector<wchar_t> buf;
	int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1,  &buf[0], 0);
	buf.resize(size);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1,  &buf[0], size);
	return std::wstring( &buf[0] );	
}

CString			Convert( const std::string& str )
{
#ifdef _UNICODE
	std::vector<wchar_t> buf;
	int size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1,  &buf[0], 0);
	buf.resize(size);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1,  &buf[0], size);
	return CString( &buf[0] );	
#else 
	return CString(str.c_str());
#endif
}

CString			Convert( const std::wstring& str )
{
#ifndef _UNICODE
	std::vector<char> buf;
	int size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str.c_str(), -1,  &buf[0], 0, 0, 0);
	buf.resize(size);
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, str.c_str(), -1,  &buf[0], size, 0, 0);
	return CString	( &buf[0] );	
#else 
	return CString(str.c_str());
#endif
}

}
