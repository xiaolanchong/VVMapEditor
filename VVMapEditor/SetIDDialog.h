//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Dialog for managing ids when loading or renumber ids in correspondence
//
//=============================================================================


#if !defined(AFX_SETIDDIALOG_H__346958D5_1EF1_4534_AA72_E6D28C708CBD__INCLUDED_)
#define AFX_SETIDDIALOG_H__346958D5_1EF1_4534_AA72_E6D28C708CBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetIDDialog.h : header file
//

#include "PropertyListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CSetIDDialog dialog

class CSetIDDialog : public CDialog
{
// Construction
	std::vector<int>	m_Ids;
public:
	CSetIDDialog(CWnd* pParent , const std::vector<int>& Ids);   // standard constructor

	CSize				m_sizeWnd;
	CPropertyListBox	m_wndProperty;

	std::vector<int>	m_Result;

// Dialog Data
	//{{AFX_DATA(CSetIDDialog)
	enum { IDD = IDD_SETID };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetIDDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void OnOK();
	// Generated message map functions
	//{{AFX_MSG(CSetIDDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CLoadIDDialog : public CDialog
{
// Construction
	std::vector<int>	m_Need;
	std::vector<int>	m_Have;
public:
	CLoadIDDialog(	CWnd* pParent , 
					const std::vector<int>&	Need,
					const std::vector<int>&	Have
					) ;  

	CSize				m_sizeWnd;
	CPropertyListBox	m_wndProperty;

	std::vector< std::pair<int, int> >	m_Result;

// Dialog Data
	//{{AFX_DATA(CSetIDDialog)
	enum { IDD = IDD_LOADID };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetIDDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();

	// Generated message map functions
	//{{AFX_MSG(CSetIDDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETIDDIALOG_H__346958D5_1EF1_4534_AA72_E6D28C708CBD__INCLUDED_)
