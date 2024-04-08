#if !defined(AFX_MERGEBAR_H__BE145F5A_BF22_4A55_80EC_B3B4FE56C016__INCLUDED_)
#define AFX_MERGEBAR_H__BE145F5A_BF22_4A55_80EC_B3B4FE56C016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeBar window

#define WM_CLOSE_MERGE	WM_USER + 23

#include <GuiControlBar.h>
#include <GuiContainer.h>
#include "MergeView.h"

class CMergeBar : public CGuiControlBar
{
// Construction
	CRect m_rcClient;
public:
	DECLARE_DYNAMIC( CMergeBar );

	CMergeBar( CWnd * pParentWnd, CString sTitle, CRect rcClient, int Index);

// Attributes
public:

	CMergeView*		m_pView;
	CGuiContainer	m_ctMerge;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeBar)
	//}}AFX_VIRTUAL


	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	CSize CalcLayout(DWORD dwMode, int nLength = -1);

	
// Implementation
public:
	virtual ~CMergeBar();
	virtual void OnCloseBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeBar)
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnAllowDocking();
	afx_msg void OnCloseB();	
};

class CMapPanoramaView;

class CMapPanoramaBar : public CGuiControlBar
{
	CMapPanoramaView* m_pView;
public:
	BOOL	Create( CWnd * pParentWnd, CString sTitle);
	void	CreateBitmap();
//	virtual void OnCloseBtn();
};

class CMapPanoramaView : public CScrollView
{
	Bitmap*	m_Bmp;
public:
	CMapPanoramaView();           // protected constructor used by dynamic creation
	virtual ~CMapPanoramaView();

	DECLARE_DYNCREATE(CMapPanoramaView)

	void	SetBitmap(Bitmap* bmp);
	CSize	CreateBitmap();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeView)
//	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEBAR_H__BE145F5A_BF22_4A55_80EC_B3B4FE56C016__INCLUDED_)
