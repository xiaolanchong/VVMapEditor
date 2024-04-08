#if !defined(AFX_POINTBAR_H__219E3E9F_2831_43E4_8737_052F8207DB2A__INCLUDED_)
#define AFX_POINTBAR_H__219E3E9F_2831_43E4_8737_052F8207DB2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointBar.h : header file
//

#include <GuiControlBar.h>
#include <GuiContainer.h>
#include <GuiTabWnd.h>
#include "PointTree.h"
#include "ExtentTree.h"

#include "sizecbar.h"

/////////////////////////////////////////////////////////////////////////////
// CPointBar window

#define baseControlBar /*CSizingControlBar*/ CGuiControlBar

class CPointBar : public baseControlBar
{
// Construction
public:
	CPointBar();


	BOOL	Create( CWnd* pParent );
// Attributes
public:

	CGuiTabWnd		m_wndTab;
	CImageList		m_ilTab;
//	CGuiContainer	m_ctPoints;
	CPointTree		m_wndPoint;
	CExtentTree		m_wndExtent;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPointBar)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTBAR_H__219E3E9F_2831_43E4_8737_052F8207DB2A__INCLUDED_)
