#if !defined(AFX_IDDIALOG_H__AC5185FB_DC5A_4871_AFA1_934C484A531D__INCLUDED_)
#define AFX_IDDIALOG_H__AC5185FB_DC5A_4871_AFA1_934C484A531D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IDDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIDDialog dialog

class CIDDialog : public CDialog
{
// Construction
public:
	CIDDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIDDialog)
	enum { IDD = IDD_ID };
	int		m_nID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIDDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIDDialog)
		// NOTE: the ClassWizard will add member functions here

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDDIALOG_H__AC5185FB_DC5A_4871_AFA1_934C484A531D__INCLUDED_)
