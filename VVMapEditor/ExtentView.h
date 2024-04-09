#if !defined(AFX_EXTENTVIEW_H__14E74ED1_CCDC_41A9_ABB8_CAB28A53CA18__INCLUDED_)
#define AFX_EXTENTVIEW_H__14E74ED1_CCDC_41A9_ABB8_CAB28A53CA18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtentView.h : header file
//

#include <memory>
#include "Helper.h"
#include "CacheDC.h"

/////////////////////////////////////////////////////////////////////////////
// CExtentView view

class CVVMapEditorDoc;

class CExtentView : public CScrollView
{
	std::unique_ptr<Bitmap>		m_Bitmap;
protected:
	CExtentView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExtentView)

// Attributes
	CCacheDC<CExtentCache>		m_ExtentDC;
public:

	int		m_Index;

	void	Update();
	CSize	GetSize();

	void	DrawHeight();
	void	InvalidateHeight() { m_ExtentDC.InvalidateCache(this);};

	CVVMapEditorDoc* GetDocument();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtentView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExtentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void	DrawHeight(CDC* pDC);
	

	// Generated message map functions
	//{{AFX_MSG(CExtentView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTENTVIEW_H__14E74ED1_CCDC_41A9_ABB8_CAB28A53CA18__INCLUDED_)
