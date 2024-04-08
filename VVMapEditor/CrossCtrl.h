#if !defined(AFX_CROSSCTRL_H__0F1EC5D9_EA24_4883_8CF9_1804AFCC42B6__INCLUDED_)
#define AFX_CROSSCTRL_H__0F1EC5D9_EA24_4883_8CF9_1804AFCC42B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CrossCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCrossCtrl window

class CCrossCtrl : public CStatic
{
// Construction
	DWORD		m_dwExtent;
	DWORD		m_dwThickness;
public:
	CCrossCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrossCtrl)
	//}}AFX_VIRTUAL

// Implementation
	void	SetThickness(DWORD dwThickness ) { m_dwThickness = dwThickness; Invalidate(); }
	void	SetExtent(DWORD dwExtent ) { m_dwExtent = dwExtent; Invalidate();}

	DWORD	GetExtent() const	{ return m_dwExtent; }
	DWORD	GetThickness() const{ return m_dwThickness;}
public:
	virtual ~CCrossCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCrossCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CROSSCTRL_H__0F1EC5D9_EA24_4883_8CF9_1804AFCC42B6__INCLUDED_)
