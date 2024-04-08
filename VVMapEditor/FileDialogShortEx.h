#if !defined(AFX_FILEDIALOGSHORTEX_H__97E943BA_14DC_454B_9888_89F31B8A6D14__INCLUDED_)
#define AFX_FILEDIALOGSHORTEX_H__97E943BA_14DC_454B_9888_89F31B8A6D14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDialogEx.h : header file
//

#include <FileDialogEx.h>

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx dialog

class CFileDialogShortEx : public CFileDialogEx
{
	DECLARE_DYNAMIC(CFileDialogShortEx)

public:
	CFileDialogShortEx
		(	BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
			LPCTSTR lpszDefExt = NULL,
			LPCTSTR lpszFileName = NULL,
			DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			LPCTSTR lpszFilter = NULL,
			CWnd* pParentWnd = NULL
			);

	bool	m_bShort;
protected:
	//{{AFX_MSG(CFileDialogShortEx)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDIALOGEX_H__97E943BA_14DC_454B_9888_89F31B8A6D14__INCLUDED_)
