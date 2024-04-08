// DllRes.cpp: implementation of the CDllRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DllResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern AFX_EXTENSION_MODULE GuiLibDLL;

CDllResource::CDllResource() : m_hDll( GuiLibDLL.hResource) {}

CString CDllResource::LoadString( UINT nID )
{
//	const int BufSize = 100;
	TCHAR	szBuf[MAX_PATH];
	::LoadString( m_hDll, nID, szBuf, MAX_PATH );
	return CString ( szBuf);
}

HICON	CDllResource::LoadIcon( UINT nID)
{
	return ::LoadIcon( m_hDll, MAKEINTRESOURCE(nID));
}

HMENU		CDllResource::LoadMenu(UINT nID)
{
	return ::LoadMenu( m_hDll,  MAKEINTRESOURCE(nID));
}

LPCDLGTEMPLATE		CDllResource::LoadDialog(UINT nID)
{
	//NOTENOTE: not necessary to free & unlock this resource
	HRSRC				hResource = ::FindResource(m_hDll, MAKEINTRESOURCE(nID), RT_DIALOG);
	HGLOBAL				hMemory = LoadResource(m_hDll, hResource);
	LPCDLGTEMPLATE		hDialogTemplate = NULL;
	if (hDialogTemplate != NULL)
		hDialogTemplate = (LPCDLGTEMPLATE)LockResource(hMemory);
	return hDialogTemplate;
}

HIMAGELIST CDllResource::LoadImageList(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
{
//	ASSERT(HIWORD(nBitmapID) == 0);
	LPCTSTR szRes = MAKEINTRESOURCE(nBitmapID);
//	ASSERT(hInst != NULL);
	return ImageList_LoadBitmap(GetHandle(), /*(LPCTSTR)nBitmapID*/szRes, cx, nGrow, crMask);
}

HBITMAP		CDllResource::LoadBitmap	( UINT nID )
{
	LPCTSTR szRes = MAKEINTRESOURCE( nID );
	return ::LoadBitmap( GetHandle(), szRes );
}