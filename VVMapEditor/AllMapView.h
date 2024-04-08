#if !defined(AFX_ALLMAPVIEW_H__01EB5FAC_043C_4785_8AAD_2A3A00C43A33__INCLUDED_)
#define AFX_ALLMAPVIEW_H__01EB5FAC_043C_4785_8AAD_2A3A00C43A33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllMapView.h : header file
//

#include "CacheDC.h"

/////////////////////////////////////////////////////////////////////////////
// CAllMapView view

class CBitmapBackground;
class CVVMapEditorDoc;

#include <memory>

class CAllMapView : public CScrollView
{
	void	DrawViewBitmap	(Graphics& gr);
	void	DrawEmptyArea	( Graphics& gr, const RectF& rc );
	CPoint	GetStartPoint() ;

//	void	DrawGetPoints( CDC* pDC );

	std::auto_ptr<CBitmapBackground>	m_Background;

	CCrossDC					m_CrossDC;
protected:
	CAllMapView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAllMapView)

// Attributes
public:

// Operations
public:
	void	DrawGetPoint();
	void	InvalidateGetPoint()	{ m_CrossDC.InvalidateCache(this); }

	void	UpdateBitmap();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllMapView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAllMapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CVVMapEditorDoc*	GetDocument();
	// Generated message map functions
	//{{AFX_MSG(CAllMapView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLMAPVIEW_H__01EB5FAC_043C_4785_8AAD_2A3A00C43A33__INCLUDED_)
