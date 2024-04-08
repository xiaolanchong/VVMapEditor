#if !defined(AFX_EXTENTBAR_H__42E76D97_9649_45EE_9B1E_E9B45CD2799C__INCLUDED_)
#define AFX_EXTENTBAR_H__42E76D97_9649_45EE_9B1E_E9B45CD2799C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtentBar.h : header file
//

#include <GuiControlBar.h>
#include "ExtentView.h"

/////////////////////////////////////////////////////////////////////////////
// CExtentBar window

#define WM_CLOSE_EXTENT	WM_USER + 24

class CExtentBar : public CGuiControlBar
{
// Construction
	CExtentView*		m_pView;
public:
	CExtentBar(CWnd * pParentWnd, CString strName, int Index);

	void		Update();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtentBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtentBar();

	virtual void OnCloseBtn();
	// Generated message map functions
protected:
	//{{AFX_MSG(CExtentBar)
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	afx_msg void OnAllowDocking();
	afx_msg void OnCloseB();	

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTENTBAR_H__42E76D97_9649_45EE_9B1E_E9B45CD2799C__INCLUDED_)
