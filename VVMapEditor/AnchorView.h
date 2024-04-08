#if !defined(AFX_ANCHORVIEW_H__0E1D3683_8F05_425A_945C_05E7C109701B__INCLUDED_)
#define AFX_ANCHORVIEW_H__0E1D3683_8F05_425A_945C_05E7C109701B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnchorView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnchorView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#include "ut/OXLayoutManager.h"
//#include "ut/OXHookWnd.h"
//#include "CustomList/ComboListCtrl.h"
/*
#pragma warning(disable : 4005)
#define max(a,b)	((a > b)?a : b	)
#define min(a,b)	((a > b)?b : a)
*/

#undef	max
#undef	min
#define max(a, b) ( (a) > (b) ? (a) : (b) )
#define min(a, b) ( (a) > (b) ? (b) : (a) )
#include "GridCtrl/GridCtrl.h"

//#pragma warning(enable: 4005)
/*
template < typename T>  class FlickItem
{
	int m_nSelectItem;


	void	SelectItem( int nItem )
	{
		m_nSelectItem = nItem;
		T.SelectItem();
	}

	void
	if ( SelectItem )
}*/

class CVVMapEditorDoc;
class CAnchorView;

class CGridCtrlEx : public CGridCtrl
{
	CAnchorView* m_pView;

    virtual void  OnEndEditCell(int nRow, int nCol, CString str);
	virtual BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);

	afx_msg	void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );

public:
	CGridCtrlEx( CAnchorView* pView ) : m_pView(pView){}
protected:
	DECLARE_MESSAGE_MAP()
};

#define CParentView CScrollView

class CAnchorView : public CParentView
{
//	COXLayoutManager	m_Layout;
	friend class CGridCtrlEx;
	BOOL			m_bInit;

	enum 
	{
		COLUMN_NUMBER	= 0,
		COLUMN_X_RO		= 1,
		COLUMN_Y_PHI	= 2,
		COLUMN_Z_PSI	= 3,
		COLUMN_RELATIVE	= 4
	};

	void	ChangeCoords(int nItem) ;
	void	InsertCoords(int nRow);
	CImageList	m_ilMapAnchor;

	CGridCtrlEx	m_wndGrid;

	int			m_nSelectedItem;
	COLORREF	m_clSelectColor;

	DWORD	m_dwRowWidth;

	int		InsertRow(int nName);

	void	OnDeleteKey( bool bShift );

	int		RowToAnchorIndex( int nRow )  const 
	{ 
		const int c_nFixeRows = 1;
		return nRow - c_nFixeRows;
	}

	int		AnchorIndexToRow( int nIndex) const 
	{
		const int c_nFixeRows = 1;
		return nIndex + c_nFixeRows;
	}
public:
	~CAnchorView();
	CAnchorView();           // protected constructor used by dynamic creation

	void	SelectItem	(int nItem);
	void	InvalidateSelectItem();
protected:
//	DECLARE_EASYSIZE
	DECLARE_DYNCREATE(CAnchorView)

// Form Data
public:
	//{{AFX_DATA(CAnchorView)
	enum { IDD = IDD_ANCHOR };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void	OnUpdateCmdUI();
	void	UpdateList();
	
    void  OnEndEditCell(int nRow, int nCol, CString str);
	BOOL  ValidateEdit(int nRow, int nCol, LPCTSTR str);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnchorView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	public:
	virtual BOOL Create(LPCTSTR, LPCTSTR, DWORD,
		const RECT&, CWnd*, UINT, CCreateContext*);

// Implementation
protected:
//	virtual ~CAnchorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

	virtual void OnDraw(CDC* pDC) {}; 

	CVVMapEditorDoc*	GetDocument();
	// Generated message map functions
	//{{AFX_MSG(CAnchorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNotifyEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) ;
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANCHORVIEW_H__0E1D3683_8F05_425A_945C_05E7C109701B__INCLUDED_)
