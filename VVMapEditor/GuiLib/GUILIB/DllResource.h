// DllRes.h: interface for the CDllRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_)
#define AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// этот класс предназначен для явной загрузки ресурсов из длл

class CDllResource
{
	HINSTANCE	m_hDll;
public:
	CDllResource();

	HINSTANCE			GetHandle() const { return m_hDll;}

	
	CString				LoadString	( UINT nID );
	HICON				LoadIcon	( UINT nID );
	HMENU				LoadMenu	( UINT nID );
	LPCDLGTEMPLATE		LoadDialog	( UINT nID );
	HIMAGELIST			LoadImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask );
	HBITMAP				LoadBitmap	( UINT nID );

};
 

#endif // !defined(AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_)
