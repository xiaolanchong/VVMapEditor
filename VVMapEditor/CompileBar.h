#if !defined(AFX_COMPILEBAR_H__BC5A81FA_0122_46C8_861F_83853170231B__INCLUDED_)
#define AFX_COMPILEBAR_H__BC5A81FA_0122_46C8_861F_83853170231B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompileBar.h : header file
//

#include "GuiStatusBar.h"

/////////////////////////////////////////////////////////////////////////////
// CCompileBar window

class CCompileBar : public CGuiStatusBar
{
	float	m_fProgress;
// Construction
public:
	CCompileBar();

	BOOL	CreateCompileBar(UINT nID);

	void	ResetProgress();
	void	SetProgress( float fPart );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompileBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCompileBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCompileBar)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPILEBAR_H__BC5A81FA_0122_46C8_861F_83853170231B__INCLUDED_)
