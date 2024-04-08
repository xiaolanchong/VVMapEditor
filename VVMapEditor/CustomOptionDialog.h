#if !defined(AFX_CUSTOMOPTIONDIALOG_H__9A9D75C5_17A5_4F90_BD9D_9FC4E6513068__INCLUDED_)
#define AFX_CUSTOMOPTIONDIALOG_H__9A9D75C5_17A5_4F90_BD9D_9FC4E6513068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomOptionDialog.h : header file
//

#include "ut/OXToolTipCtrl.h"
#include "CrossCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomOptionDialog dialog

//class CVVMapEditorDoc;

class CCustomOptionPage : public CPropertyPage
{
	COXToolTipCtrl		m_wndToolTip;
	CCrossCtrl			m_wndCross;
//	CVVMapEditorDoc*	m_pDoc;
// Construction
public:
	//NOTENOTE: may use custom settings structure , but currently it's CVVMapEditorDoc::Settings
	CCustomOptionPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomOptionDialog)
	enum { IDD = IDD_OPTIONS };
	CSpinButtonCtrl	m_wndThickness;
	CSpinButtonCtrl	m_wndExtent;
	CSpinButtonCtrl	m_wndSpinMRU;
	UINT	m_nMRU;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomOptionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage( MSG *pMsg );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomOptionDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditExtent();
	afx_msg void OnChangeEditThickness();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/*
class CCustomOptionPage : public CCustomOptionDialog
{

};
*/
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMOPTIONDIALOG_H__9A9D75C5_17A5_4F90_BD9D_9FC4E6513068__INCLUDED_)
