// VVMapEditor.h : main header file for the VVMAPEDITOR application
//

#if !defined(AFX_VVMAPEDITOR_H__62AD261A_6EEF_49B9_9AD7_C0576E44C5D9__INCLUDED_)
#define AFX_VVMAPEDITOR_H__62AD261A_6EEF_49B9_9AD7_C0576E44C5D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/*
#include "gdiplus.h"
using namespace Gdiplus;
*/
/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorApp:
// See VVMapEditor.cpp for the implementation of this class
//

class CVVMapEditorApp : public CWinApp
{
	class GdiPlusInit
	{
		ULONG_PTR					gdiplusToken;
	public:
		GdiPlusInit()
		{
			GdiplusStartupInput gdiplusStartupInput;
			// Initialize GDI+.
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}
		~GdiPlusInit()
		{
			GdiplusShutdown(gdiplusToken);
		}
	}	m_GdiInit;

public:
	CVVMapEditorApp();

	bool	m_bShort;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVVMapEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

//	void	SaveMRU();
	void	ReloadMRU(int nMRUNumber);

// Implementation
	//{{AFX_MSG(CVVMapEditorApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VVMAPEDITOR_H__62AD261A_6EEF_49B9_9AD7_C0576E44C5D9__INCLUDED_)
