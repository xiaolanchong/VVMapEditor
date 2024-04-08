#if !defined(AFX_CREATECADLG_H__8BC3CC38_CEE4_4E9B_8CCD_5CBF4F3BC4BE__INCLUDED_)
#define AFX_CREATECADLG_H__8BC3CC38_CEE4_4E9B_8CCD_5CBF4F3BC4BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateCADlg.h : header file
//

#include <vector>
#include "SortListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CCreateCADlg dialog

class CCreateCADlg : public CDialog
{
// Construction

public:
	CCreateCADlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateCADlg)
	enum { IDD = IDD_CREATE_AREA };
protected:
	CSortListBox	m_listSelect;
	CSortListBox	m_listUnselect;
	//}}AFX_DATA

	std::vector<int>		m_All;
	std::vector<int>		m_Select;
	std::pair<bool,bool>	m_Map;
public:
//	std::vector<CString>	m_All;

	void					SetAllMasters(const std::vector<int>& Ids)		{ m_All = Ids;		};
	void					SetSelectMasters(const std::vector<int>& Ids)	{ m_Select = Ids;	};
	void					SetMap(bool bMap, bool bSelect)					{ m_Map = std::make_pair( bMap, bSelect);};
	std::pair<bool,bool>	IsMapSelect() const								{ return m_Map;		};
	std::vector<int>		GetMasters() const								{ return m_Select;	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateCADlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateCADlg)
	afx_msg void OnAdd();
	afx_msg void OnAddAll();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg int OnCompareItem( int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATECADLG_H__8BC3CC38_CEE4_4E9B_8CCD_5CBF4F3BC4BE__INCLUDED_)
