#if !defined(AFX_EXTENTTREE_H__4D780F15_83D2_4B21_962B_C7961253C6BD__INCLUDED_)
#define AFX_EXTENTTREE_H__4D780F15_83D2_4B21_962B_C7961253C6BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtentTree.h : header file
//

#include "ut/OxTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CExtentTree window

class CExtentTree : public COXTreeCtrl
{
// Construction
public:
	CExtentTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtentTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtentTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtentTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTENTTREE_H__4D780F15_83D2_4B21_962B_C7961253C6BD__INCLUDED_)
