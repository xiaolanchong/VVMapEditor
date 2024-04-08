#if !defined(AFX_MERGEVIEW_H__E139A939_7EF0_4738_8055_247D49582B5C__INCLUDED_)
#define AFX_MERGEVIEW_H__E139A939_7EF0_4738_8055_247D49582B5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergeView view

class CMergeView : public CScrollView
{
public:
	CMergeView();           // protected constructor used by dynamic creation
	virtual ~CMergeView();
	DECLARE_DYNCREATE(CMergeView)



// Attributes
public:

	void	SetBitmap(Bitmap* bmp);
	CSize	CreateBitmap();

	Bitmap*		m_Bmp;
	int			m_Index;
	CRect		m_rcArea;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeView)
//	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMergeView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEVIEW_H__E139A939_7EF0_4738_8055_247D49582B5C__INCLUDED_)
