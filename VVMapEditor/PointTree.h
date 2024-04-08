#if !defined(AFX_POINTTREE_H__73212E76_235A_4237_803D_E072BA19886C__INCLUDED_)
#define AFX_POINTTREE_H__73212E76_235A_4237_803D_E072BA19886C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPointTree window
#include "ut/OXTreeCtrl.h"
#include <utility>


class CPointTree : public /*CTreeCtrl*/COXTreeCtrl
{
// Construction
	std::pair<bool, int>	m_nSelectGroup;
	void	InvertGroupColor( int nGroup );

	HTREEITEM	InsertGroup(int nGroup);
public:
	CPointTree();

	void	Refresh();
	void	InvertGroup(int nGroup);
	void	InvalidateInvertGroup();
// Attributes
public:

	CMenu	m_Menu;
	CPoint	m_ClickPoint;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPointTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGroupDelete();
	afx_msg void OnPointDelete();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTTREE_H__73212E76_235A_4237_803D_E072BA19886C__INCLUDED_)
