#if !defined(AFX_MERGEDIALOG_H__2CA3B7F1_8F23_44C3_B905_C7BEF964551F__INCLUDED_)
#define AFX_MERGEDIALOG_H__2CA3B7F1_8F23_44C3_B905_C7BEF964551F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeDialog.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog dialog

class CMergeDialog : public CDialog
{
// Construction
	std::vector<int>	m_Ids;
public:
	CMergeDialog(CWnd* pParent , const std::vector<int>& Ids);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMergeDialog)
	enum { IDD = IDD_MERGE };
	CComboBox	m_wndRight;
	CComboBox	m_wndLeft;
	//}}AFX_DATA


	int m_nLeft, m_nRight;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMergeDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDIALOG_H__2CA3B7F1_8F23_44C3_B905_C7BEF964551F__INCLUDED_)
