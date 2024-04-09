//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: CVVMapEditorView is map view class of main window. it holds map image & 
//			points too
//
//=============================================================================

// VVMapEditorView.h : interface of the CVVMapEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VVMAPEDITORVIEW_H__E8C95CD3_D9CC_4F22_A297_C1C9AA29E318__INCLUDED_)
#define AFX_VVMAPEDITORVIEW_H__E8C95CD3_D9CC_4F22_A297_C1C9AA29E318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FigPolyline.h"
#include "PicBackground.h"
#include <memory>
#include <GuiView.h>

#include "CacheDC.h"
#include "StateCache.h"
#include "TfxdataTip.h"
/*
struct CToolTipEx : public COXToolTipCtrl
{
	void DisplayToolTip(CPoint& pt, BOOL bExtended = FALSE)
	{
		COXToolTipCtrl::DisplayToolTip( pt, bExtended );
	}
};*/

class CVVMapEditorView : public CScrollView
{
protected: // create from serialization only
	CVVMapEditorView();
	DECLARE_DYNCREATE(CVVMapEditorView)

// Attributes

	struct EditorState
	{
		enum State
		{
			NONE,
			EDIT,
/*			EDIT_IZ,
			EDIT_MO*/
		};
		State	m_State;
		PointF	m_Point;
		int		m_Number;

		std::unique_ptr<CFigPolyline>	m_CurrentPoly;
		enum { ERR_POLY = -1};
		int		m_nActivePoly;

		EditorState() : m_State(NONE), m_nActivePoly(ERR_POLY){}
	} m_EditorState;

	int		m_nSelectGroup;
	
	int		m_nHilitedAnchorPoint;
	TFXDataTip	m_wndToolTip;

	CPoint	GetStartPoint() const
	{
		CRect rc;
		GetClientRect(&rc);
		if(!m_Background.get()) return CSize(0,0);
		CSize size = m_Background->GetSize();
		size.cx *= m_Scale;
		size.cy *= m_Scale;
		int X = rc.CenterPoint().x - size.cx/2;
		int Y = rc.CenterPoint().y - size.cy/2;
		X = X > 0 ? X : 0;
		Y = Y > 0 ? Y : 0;
		return CSize(X, Y);
	}

//	CPoint	GetScalePoint( PointF ptf);

	CRect	GetImageRect();
	void	StartEdit();
	void	EndEdit();

	void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	bool	CursorInRect() ;
//	void	AddMenuItem(  )


	CVVMapEditorDoc* GetDocument();

//-----------------------------------------------------------------------------
// Purpose: render from OnDraw
// Input  : gr - Graphics backsreen buffer
//-----------------------------------------------------------------------------
	void	DrawPoints		(Graphics& gr/*, const CRect& rcClient*/);
	void	DrawPolygons	(Graphics& gr);
	void	DrawGetPoints	(Graphics& gr);
	void	DrawIZ			(Graphics& gr);
	void	DrawMapOrigin	(Graphics& gr);
	void	DrawAnchorPoints(Graphics& gr);
	void	DrawRefPoints	(Graphics& gr);
	void	DrawPointGroup( Graphics& gr, PointF point, int Number, Brush& br);
	void	DrawPointNumber( Graphics& gr, PointF point, int Number );
//	void	DrawViewBitmap	(Graphics& gr);

	void	DrawGetPoints( CDC* pDC );

	void	Scale(float Scale);
//	PointF	ScalePoint(CPoint point) const;

	void	StartEditIZ();
	void	EndEditIZ();
	void	CancelEditIZ();

	PointF	MousePointToMapPoint(CPoint point) const;
	CPoint	MapPointToMousePoint(PointF point) const;

	void	Escape();
	void	FillAnchorMenu( CMenu& menu );

	void	UpdateCoords( CPoint ptMouse );
	void	InvalidateCoords();

	// 2004/04/21 - new functions for map stuff
//	bool	SetMapInfo();
	bool	GetMapInfo();

//-----------------------------------------------------------------------------
// Purpose: find anchor point from device point, return number or -1
// Input  : point - mouse point form handler
//-----------------------------------------------------------------------------
	int					FindAnchorPointOnMap(CPoint point);
	int					FindCorrPointOnMap( CPoint point );

	std::unique_ptr<CBackground>	m_Background;

	HCURSOR		m_hCross;

	float		m_Scale;

	CMenu		m_GroupMenu;
	CCrossDC					m_CrossDC;
	CCacheDC<CPointCache>		m_PointDC;

	CCacheDC<CAnchorCache>		m_AnchorDC;
	CCacheDC<CAnchorCache>		m_RefDC;

	CStateCache					m_StateCache;

public:
	const CBackground*	GetBackground() const { return m_Background.get();}
	CBackground*		GetBackground()  { return m_Background.get();}

	void	DrawGetPoint();
	void	DrawSelectPoint(int GroupNumber);
	void	DrawAnchorPoint(int nPoint);
	void	DrawRefPoint(int nPoint);

	void	InvalidateGetPoint()	{ m_CrossDC.InvalidateCache(this); }
	void	InvalidateSelectPoint() { m_PointDC.InvalidateCache(this); }
	void	InvalidateAnchorPoint() { m_AnchorDC.InvalidateCache(this); }
	void	InvalidateRefPoint()	{ m_RefDC.InvalidateCache(this); }

//	CToolBar		m_wndToolBar;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVVMapEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVVMapEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVVMapEditorView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMapImage();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMapExport();
	afx_msg void OnUpdateMapExport(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	afx_msg void OnDynamicGroupMenu( UINT nID );
	afx_msg void OnDynamicAnchorMenu( UINT nID );	
	afx_msg void OnNotifyTooltip(NMHDR * pNMHDR, LRESULT * result);
	
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VVMapEditorView.cpp
inline CVVMapEditorDoc* CVVMapEditorView::GetDocument()
   { return (CVVMapEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VVMAPEDITORVIEW_H__E8C95CD3_D9CC_4F22_A297_C1C9AA29E318__INCLUDED_)

