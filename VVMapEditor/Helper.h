//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: For unicode <-> mbcs conversion
//
//=============================================================================


// Helper.h: interface for the Helper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELPER_H__D4E6AAC6_4C5D_44F1_BF4E_6ADB097A959E__INCLUDED_)
#define AFX_HELPER_H__D4E6AAC6_4C5D_44F1_BF4E_6ADB097A959E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace Helper  
{
	std::string		ConvertA(LPCWSTR szStr);
	std::wstring	ConvertW(LPCWSTR szStr);

	std::string		ConvertA(LPCSTR  szStr);
	std::wstring	ConvertW(LPCSTR  szStr);
	
	CString			Convert( const std::string& str );
	CString			Convert( const std::wstring& str );

}

#endif // !defined(AFX_HELPER_H__D4E6AAC6_4C5D_44F1_BF4E_6ADB097A959E__INCLUDED_)

