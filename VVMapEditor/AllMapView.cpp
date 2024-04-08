// AllMapView.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "AllMapView.h"
#include "VVMapEditorDoc.h"
#include "PicBackground.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllMapView

IMPLEMENT_DYNCREATE(CAllMapView, CScrollView)

CAllMapView::CAllMapView()
{
}

CAllMapView::~CAllMapView()
{
}

const int IDT_GetPoint = 1;


BEGIN_MESSAGE_MAP(CAllMapView, CScrollView)
	//{{AFX_MSG_MAP(CAllMapView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllMapView drawing

void CAllMapView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

/*	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);*/
	UpdateBitmap();
}

void CAllMapView::OnDraw(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	// TODO: add draw code here
		//FIXME: stub
		const float m_Scale = 1.0f;
	CRect rc;
	GetClientRect(&rc);

//	Bitmap*	pBitmap = pDoc->GetViewBitmap();

	CSize size = m_Background.get() ? m_Background->GetSize() : CSize(0,0);
	size.cx *= m_Scale;
	size.cy *= m_Scale;

	size.cx = max( rc.Width(), size.cx );
	size.cy = max( rc.Height(), size.cy );

	Graphics	Native(pDC->GetSafeHdc());
	Bitmap		BackBmp(size.cx, size.cy);
	Graphics	BackBuffer(&BackBmp);

	m_CrossDC.InvalidateCache(this);
	BackBuffer.Clear(Color::LightGray);
	if( m_Background.get() )
	{
		DrawViewBitmap(BackBuffer);
	}
	else
	{
		//FIXME: scale
		const float m_Scale = 1.0f;
		CSize size ( rc.Size());
		CPoint st = GetStartPoint();
		size.cx *= m_Scale;
		size.cy *= m_Scale;
		CRectF rcBG( st.x, st.y, size.cx, size.cy );

		DrawEmptyArea( BackBuffer, rcBG );
	}
	Native.DrawImage( &BackBmp, 0, 0, size.cx, size.cy );
}

/////////////////////////////////////////////////////////////////////////////
// CAllMapView diagnostics

#ifdef _DEBUG
void CAllMapView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAllMapView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

CVVMapEditorDoc*	CAllMapView::GetDocument()
{
	return (CVVMapEditorDoc*)m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CAllMapView message handlers

void CAllMapView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class

	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CAllMapView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CScrollView::OnEraseBkgnd(pDC);
}

void	CAllMapView::DrawViewBitmap	(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
//	Bitmap* pBitmap = pDoc->GetViewBitmap();
	if( m_Background.get() )
	{
		//FIXME: stub
		const float m_Scale = 1.0f;
		CSize size = m_Background->GetSize();
		CPoint st = GetStartPoint();
		size.cx *= m_Scale;
		size.cy *= m_Scale;
		CRectF rcBG( st.x, st.y, size.cx, size.cy );
//		gr.DrawImage( pBitmap, rcBG );
		m_Background->Draw( gr, rcBG );
	}
}

CPoint	CAllMapView::GetStartPoint() 
{
		//FIXME: stub
	const float m_Scale = 1.0f;
	CVVMapEditorDoc* pDoc = GetDocument();
	CRect rc;
	GetClientRect(&rc);
	if(!m_Background.get() ) return CSize(0,0);
	CSize size  ( m_Background->GetSize() ) ;
	size.cx *= m_Scale;
	size.cy *= m_Scale;
	int X = rc.CenterPoint().x - size.cx/2;
	int Y = rc.CenterPoint().y - size.cy/2;
	X = X > 0 ? X : 0;
	Y = Y > 0 ? Y : 0;
	return CSize(X, Y);
}

void CAllMapView::DrawEmptyArea(Graphics& gr, const RectF& rc) 
{
   FontFamily  fontFamily(L"Arial");
   Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
   
   StringFormat stringFormat;
   stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetAlignment(StringAlignmentCenter);

	//FIXME: Hack on unicode
	CString strNoImage;
	strNoImage.LoadString( IDS_NO_IMAGE );
	std::wstring str = strNoImage;		
	gr.DrawString( str.c_str(), -1, &font, rc, &stringFormat, &SolidBrush( Color( 0, 0, 0)));


}

void CAllMapView::UpdateBitmap()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	//NOTENOTE: caution
	m_Background = pDoc->CreateViewBitmap();
	CSize size = m_Background.get()? m_Background->GetSize() : CSize(0,0);
	SetScrollSizes( MM_TEXT, size );
}

void	CAllMapView::DrawGetPoint()
{
	//FIXME: 
	const float m_Scale = 1.0f;
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent( ) &&
		pDoc->m_bShowGetPoints)
	{
		long x = pDoc->m_GetPoint.second.x;
		long y = pDoc->m_GetPoint.second.y;
		long xto, yto;

		int Index = pDoc->m_Ids.size();
		if( pDoc->m_GetPoint.first == Index ) 
		{
			m_CrossDC.InvalidateCache(this);
			return;
		}
		long err = pDoc->m_VVMapTrans->GetXY( pDoc->m_GetPoint.first, x, y, Index, &xto, &yto  );

		CPoint Start = GetStartPoint();
//		Start.Offset( -GetScrollPosition() );
		xto = xto * m_Scale + Start.x;
		yto = yto * m_Scale + Start.y;

		CSize sizeCross (	GetSettings().GetProperty( prop_CrossExtent ),
							GetSettings().GetProperty( prop_CrossThickness ) );

		CCrossCache cache( CPoint(xto, yto), err == ERR_NO_ERROR/*0*/, sizeCross.cx/2, sizeCross.cy  );
		m_CrossDC.Draw( this, cache );
	}	
}

void CAllMapView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CVVMapEditorDoc* pDoc = GetDocument();
	if(	pDoc->m_bShowGetPoints)
	{
		PointF CurPoint(point.x, point.y);
		CPoint Start = GetStartPoint();
		CSize sizeScroll = GetScrollPosition();
		CPoint pt = point - Start + sizeScroll;
		pt.x /= 1.0f/*m_Scale*/;
		pt.y /= 1.0f/*m_Scale*/;

		int Index = pDoc->m_Ids.size();
		pDoc->m_GetPoint = std::make_pair( Index, pt );
//		m_EditorState.m_Point = CurPoint;
		pDoc->DrawGetPoint();
		SetTimer( IDT_GetPoint, 100, 0 );
	}

	CScrollView::OnMouseMove(nFlags, point);
}

bool	CursorInRect( CView* pView  )
{
		CRect rc;
		CPoint pt;
		GetCursorPos( &pt );
		pView->GetClientRect( &rc );
		pView->ScreenToClient(&pt);
		return rc.PtInRect( pt ) == TRUE;
}

void CAllMapView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	bool bInRect = CursorInRect(this);
	if( nIDEvent == IDT_GetPoint )
	{
		KillTimer( IDT_GetPoint );	
		if( !bInRect)
		{
			m_CrossDC.InvalidateCache(this);
		}
	}


	CScrollView::OnTimer(nIDEvent);
}

