// FileDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "FileDialogShortEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx

IMPLEMENT_DYNAMIC(CFileDialogShortEx, CFileDialogEx)

CFileDialogShortEx::CFileDialogShortEx
	(	BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialogEx(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd),
	m_bShort(false)
{
}


BEGIN_MESSAGE_MAP(CFileDialogShortEx, CFileDialogEx)
	//{{AFX_MSG_MAP(CFileDialogShortEx)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CFileDialogShortEx::OnDestroy() 
{
	CFileDialogEx::OnDestroy();
	
	// TODO: Add your message handler code here
	m_bShort = IsDlgButtonChecked(IDC_SHORT) != 0;
}
