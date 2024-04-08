#if !defined(AFX_MAPSCALEVIEW_H__D8CF17E3_A2A2_4723_822F_AB0CE8F07387__INCLUDED_)
#define AFX_MAPSCALEVIEW_H__D8CF17E3_A2A2_4723_822F_AB0CE8F07387__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapScaleView.h : header file
//

#include "VVMapEditorDoc.h"
#include "CustomList/ComboListCtrl.h"
//#include "UT/OXTreeCtrl.h"
#include "EditTreeCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CMapScaleView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CVVMapEditorDoc;

class CMapScaleView : public CFormView
{
	bool		m_bInit;
	CMenu		m_menuNew;
	HTREEITEM	m_htiRootMenu;

	enum
	{
		COL_POINT		= 0,
		COL_DISTANCE	= 1
	};
	CFont		m_fnObject;
	CFont		m_fnPoint;
	CFont		m_fnDistance;

	typedef std::map< HTREEITEM, std::pair<int, int> >	TreeMap_t;
	void	FillTreeMap( TreeMap_t& tm );
	void	FillParentMap(  std::set<HTREEITEM> & Parents);

	HTREEITEM	AddObject(int Number);
	void		AddPoint(HTREEITEM htiParent, int nPoint, double fDistance);
protected:
	CMapScaleView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMapScaleView)

// Form Data
public:
	//{{AFX_DATA(CMapScaleView)
	enum { IDD = IDD_MAP_SCALE };
	/*COXTreeCtrl*/CEditTreeCtrl		m_wndObject;

	CString	m_strEditX;
	CString	m_strEditY;

	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void	UpdateTree();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapScaleView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
public:
	virtual BOOL Create(LPCTSTR, LPCTSTR, DWORD,
		const RECT&, CWnd*, UINT, CCreateContext*);
	void	OnUpdateCmdUI();

// Implementation
protected:
	virtual ~CMapScaleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CVVMapEditorDoc*	GetDocument();
	// Generated message map functions
	//{{AFX_MSG(CMapScaleView)
	afx_msg void OnChangeEditX();
	afx_msg void OnChangeEditY();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclickListObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditListObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListObject(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg	void OnMenuNew();
	afx_msg	LRESULT OnFillEdit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPSCALEVIEW_H__D8CF17E3_A2A2_4723_822F_AB0CE8F07387__INCLUDED_)
