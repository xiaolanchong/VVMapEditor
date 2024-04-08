#if !defined(AFX_MENUVIEW_H__62E8BCE5_AFD3_45A3_A7B8_C3D405757F33__INCLUDED_)
#define AFX_MENUVIEW_H__62E8BCE5_AFD3_45A3_A7B8_C3D405757F33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MenuView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMenuView view

class CMenuView : public CView
{
protected:
	CMenuView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMenuView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMenuView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMenuView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMenuView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MENUVIEW_H__62E8BCE5_AFD3_45A3_A7B8_C3D405757F33__INCLUDED_)
