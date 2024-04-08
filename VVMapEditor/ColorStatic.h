//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: control for color settings drawing
//
//=============================================================================

#if !defined(AFX_COLORSTATIC_H__A7B26B4D_1C81_481E_8ED7_61956AB7A00E__INCLUDED_)
#define AFX_COLORSTATIC_H__A7B26B4D_1C81_481E_8ED7_61956AB7A00E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatic window

class CColorStatic : public CStatic
{
// Construction
	DWORD	m_dwNumber;

	CRect	GetColorRect();
	CRect	GetTextRect();
public:
	CColorStatic( DWORD dwNumber );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatic)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorStatic)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATIC_H__A7B26B4D_1C81_481E_8ED7_61956AB7A00E__INCLUDED_)

