#if !defined(AFX_COLOROPTIONPAGE_H__F9FF2D5C_3C72_437F_818B_BD07D99BF27B__INCLUDED_)
#define AFX_COLOROPTIONPAGE_H__F9FF2D5C_3C72_437F_818B_BD07D99BF27B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorOptionPage.h : header file
//
#include "ColorListCtrl.h"
#include "ColorStatic.h"
/////////////////////////////////////////////////////////////////////////////
// CColorOptionPage dialog

class CColorOptionPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorOptionPage)

// Construction
public:
	CColorOptionPage();
	~CColorOptionPage();

// Dialog Data
	//{{AFX_DATA(CColorOptionPage)
	enum { IDD = IDD_COLOR_OPTIONS };
	CColorStatic	m_wndStExtentColor;
	CColorStatic	m_wndStPointNumberColor;
	CColorStatic	m_wndStRefPointColor;
	CColorStatic	m_wndStMapAnchorColor;
	CColorStatic	m_wndStAnchorColor;
	CColorListCtrl	m_wndPoint;
	CColorListCtrl	m_wndMaster;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorOptionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorOptionPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOROPTIONPAGE_H__F9FF2D5C_3C72_437F_818B_BD07D99BF27B__INCLUDED_)
