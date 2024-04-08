//======================= Copyright © 2004, Eugene V. Gorbachev ===============
//
// Purpose: since COXTreeCtrl eat LVN_BEGINLABELEDIT & don't send it to parent
//			we derive our own class
//
//=============================================================================
#if !defined(AFX_EDITTREECTRL_H__563ECB52_A857_4D69_9F5C_6BB3CBE839D6__INCLUDED_)
#define AFX_EDITTREECTRL_H__563ECB52_A857_4D69_9F5C_6BB3CBE839D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditTreeCtrl.h : header file
//
#include "UT/OXTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CEditTreeCtrl window

#define WM_FILL_EDIT	WM_USER + 0x11

class CEditTreeCtrl : public COXTreeCtrl
{
// Construction
public:
	CEditTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditTreeCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTREECTRL_H__563ECB52_A857_4D69_9F5C_6BB3CBE839D6__INCLUDED_)
