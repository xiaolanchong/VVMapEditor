#if !defined(AFX_PICVIEW_H__E0EF08E5_CBCC_408E_B112_351CDFBFB79F__INCLUDED_)
#define AFX_PICVIEW_H__E0EF08E5_CBCC_408E_B112_351CDFBFB79F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicView.h : header file
//


/************************************
  Revised on 25.08.2004 18:18:32
  Comments: ƒобавлено отображение высот по таблице размеров
 ************************************/


#include "Background.h"
#include "CoolMenu.h"
#include <memory>

//#include "..\Servers\Common\Mapimg\mapimg.h"
#ifdef USE_SRS
#include "..\SRS\SrsMulticam\srslib\IAnalyzer.h" // "SRS\SRSMULTICAM\SRSLib\IAnalyzer.h"
#endif
#include "CacheDC.h"
#include "TFXDataTip.h"

#include "StateCache.h"

#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CameraView.h view

class CVVMapEditorDoc;

class CCameraView : public CScrollView
{
	TFXDataTip					m_wndToolTip;
protected:
	CCameraView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCameraView)

private:
	class ItemEditState
	{
	protected:
		PointF	m_ptfOldMouse;
	public:
		ItemEditState(PointF ptf):m_ptfOldMouse(ptf){}
		virtual void	SetMousePoint(PointF ptf) { m_ptfOldMouse = ptf; };
		PointF	GetMousePoint() const { return m_ptfOldMouse;}
		virtual ~ItemEditState() {};
	};

	class PointEditState : public ItemEditState
	{
	public:
		PointEditState(PointF ptf):ItemEditState(ptf){}
	};

	class PointSelEditState : public ItemEditState
	{
		PointF&	m_EditPoint;
	public:
		PointSelEditState(PointF& ptf):ItemEditState(ptf), m_EditPoint(ptf){}
//		int	GetNumber() const { return m_Number;}

		virtual void	SetMousePoint(PointF ptf)
		{
			m_EditPoint = ptf;
			ItemEditState::SetMousePoint(ptf);
		}
	};

	class ExtentEditState : public ItemEditState
	{
	protected:
		ExtentEditState(PointF ptf):ItemEditState(ptf){}
	};

	class ExtentSelectedEditState : public ExtentEditState
	{
		int		m_ExtNumber;
		RectF&	m_EditRect;
		int		m_EditConstraint;
		PointF	m_ptfStatic, m_ptfDynamic;

		void	UpdateRect();
	public:
		int				GetSelectExtent() const { return m_ExtNumber;}
		virtual void	SetMousePoint(PointF ptf);
		int				GetConstraint() const { return m_EditConstraint; }

		ExtentSelectedEditState( PointF ptf, int Number, RectF& EditRect, int Constraint);
	};

	class ExtentNewEditState : public ExtentEditState
	{
		PointF	m_ptfStartMouse;
	public:
	
		RectF	GetCurrentRect() const;
		ExtentNewEditState(PointF ptf):ExtentEditState(ptf), m_ptfStartMouse(ptf){}
	};	

	std::unique_ptr<CBackground>	m_Background;
	bool						m_bStretch;

	std::vector<CRect>			m_Rects;

	std::pair< bool, int>		m_MergedPair;		

	struct EditorState
	{
		enum State
		{
			NONE,
			EDIT
		};
		State	m_State;
		std::unique_ptr<ItemEditState> m_Edit;
/*		PointF	m_MousePoint;
		PointF	m_RectPoint[2];
		int		m_ExtNumber;

		int		m_Number;*/

		EditorState() : m_State(NONE){}
	} m_EditorState;

	CMenu						m_GroupMenu;

	int							m_nSelectGroup;

	bool						m_bSrs;

	HCURSOR						m_hArrow[8];
	HCURSOR						m_hCross;
#ifdef USE_SRS
	CIAnalyzer *				m_pAnalyzer;
#endif

	CCrossDC					m_CrossDC;
	CCacheDC<CPointCache>		m_PointDC;
	CCacheDC<CExtentCache>		m_ExtentDC;

	CStateCache					m_StateCache;

	int					FindCorrPointOnMap( CPoint point );
// Attributes
public:

	void	Resize(CSize size);
	void	Stop();
	void	Play();
	void	Pause();

	void	DrawGetPoint();

	void	InvalidateGetPoint()	{ m_CrossDC.InvalidateCache(this); m_StateCache.Invalidate(this); }

	void	DrawHeight();
	void	InvalidateHeight() { m_ExtentDC.InvalidateCache(this);};

	void	DrawSelectPoint(int GroupNumber);
	void	InvalidateSelectPoint() { m_PointDC.InvalidateCache(this); }

	int		m_Number;

	const CBackground* GetBackground() const { return m_Background.get();}
	CBackground* GetBackground()  { return m_Background.get();}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CameraView.h)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	DrawPoints(Graphics& gr, const CRect& rcClient);
	void	DrawPolygons(Graphics& gr);
	void	DrawExtents(Graphics& gr);
	void	DrawSRSPoints( Graphics& gr, Bitmap& bmp );

	void	DrawPointGroup(	Graphics& gr, 
							PointF point, 
							int Number,
							Brush& br
							);
	void	DrawPointNumber( Graphics& gr, const CRect& rcClient, int Number);
	
//	void	DrawHeights(Graphics& gr);
	void	DrawGetPoints(Graphics& gr);
	void	DrawGetPoints(CDC* pDC);
	void	DrawHeight(CDC* pDC);

	void	DrawHoryzon( Graphics& gr );

	int		PtInExtent(CPoint pt);

	void	ResetAllSettings();
	void	StartEdit();
	void	EndEdit();
	void	SetScroll();

	CRect GetStartPoint();

public:
	CVVMapEditorDoc* GetDocument();
protected:
	virtual ~CCameraView();

	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCameraView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMasterImage();
	afx_msg void OnMasterVideo();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMasterSrs();
	afx_msg void OnUpdateMasterSrs( CCmdUI* pCmdUI);
	afx_msg void OnMasterSetid();
	afx_msg void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );	
	afx_msg void OnMasterCamera();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMasterEmtpy();
	afx_msg void OnMasterOpenTable();
	afx_msg void OnMasterSaveTable();
	afx_msg void OnCancelMode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnVideoStop(WPARAM wParam, LPARAM lParam); 
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	//}}AFX_MSG
	
	afx_msg void OnNotifyTooltip(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void OnDynamicGroupMenu( UINT nID );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICVIEW_H__E0EF08E5_CBCC_408E_B112_351CDFBFB79F__INCLUDED_)
