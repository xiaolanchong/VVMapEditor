#if !defined(AFX_ANCHORDLG_H__0C7ABA3D_DC16_4B65_B884_F4D65BCB7C82__INCLUDED_)
#define AFX_ANCHORDLG_H__0C7ABA3D_DC16_4B65_B884_F4D65BCB7C82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnchorDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CAnchorDlg dialog

class CAnchorDlg : public CDialog
{
// Construction
public:
	CAnchorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnchorDlg)
	enum { IDD = IDD_ANCHOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnchorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnchorDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANCHORDLG_H__0C7ABA3D_DC16_4B65_B884_F4D65BCB7C82__INCLUDED_)
