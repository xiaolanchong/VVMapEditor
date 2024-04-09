// PicView.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"

#include "VVMapEditorDoc.h"
#include "CameraView.h"

#include "EmptyBackground.h"
#include "PicBackground.h"
#include "VideoBackground.h"
#include "RealtimeBackground.h"

#include "Helper.h"

//#include "Mapimg.h"
#include "FileDialogShortEx.h"
#include <math.h>

#include "SetIDDialog.h"
#include "DrawHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using Helper::Convert;
using Helper::ConvertA;
using Helper::ConvertW;


const int IDT_GetPoint		= 1;
const int IDT_SelectPoint	= 2;
const int IDT_HeightPoint	= 3;

#define MY_max(a,b)    (((a) > (b)) ? (a) : (b))
#define MY_min(a,b)    (((a) < (b)) ? (a) : (b))

/////////////////////////////////////////////////////////////////////////////
// CCameraView

IMPLEMENT_DYNCREATE(CCameraView, CScrollView)

CCameraView::CCameraView() : 
	m_Background( new CEmptyBackground() ),
	m_bStretch(false),
	m_hCross( AfxGetApp()->LoadCursor( IDC_CROSS2 ) ),
	m_bSrs(false),
#ifdef USE_SRS
	m_pAnalyzer(0),
#endif
	m_MergedPair( std::make_pair(false, -1))
{
#ifdef USE_SRS
	GetAnalyzerInterface(&m_pAnalyzer);
	m_pAnalyzer->Init( "", NULL, -1 );
#endif

	m_hArrow[ CVVMapEditorDoc::top		]	= ::LoadCursor( 0, IDC_SIZENS );
	m_hArrow[ CVVMapEditorDoc::bottom	]	= ::LoadCursor( 0, IDC_SIZENS );
	m_hArrow[ CVVMapEditorDoc::topleft	]	= ::LoadCursor( 0, IDC_SIZENWSE );
	m_hArrow[ CVVMapEditorDoc::bottomright]	= ::LoadCursor( 0, IDC_SIZENWSE );
	m_hArrow[ CVVMapEditorDoc::topright]	= ::LoadCursor( 0, IDC_SIZENESW );
	m_hArrow[ CVVMapEditorDoc::bottomleft]	= ::LoadCursor( 0, IDC_SIZENESW );
	m_hArrow[ CVVMapEditorDoc::left]		= ::LoadCursor( 0, IDC_SIZEWE );
	m_hArrow[ CVVMapEditorDoc::right]		= ::LoadCursor( 0, IDC_SIZEWE );
}

CCameraView::~CCameraView()
{
#ifdef USE_SRS
	m_pAnalyzer->_Release();
#endif
}


BEGIN_MESSAGE_MAP(CCameraView, CScrollView)
	//{{AFX_MSG_MAP(CCameraView)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MASTER_IMAGE, OnMasterImage)
	ON_COMMAND(ID_MASTER_VIDEO, OnMasterVideo)
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_MASTER_SRS, OnMasterSrs)
	ON_UPDATE_COMMAND_UI(ID_MASTER_SRS, OnUpdateMasterSrs)
	ON_COMMAND(ID_MASTER_SETID, OnMasterSetid)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_MASTER_CAMERA, OnMasterCamera)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_MASTER_EMTPY, OnMasterEmtpy)
	ON_COMMAND(ID_MASTER_OPEN_TABLE, OnMasterOpenTable)
	ON_COMMAND(ID_MASTER_SAVE_TABLE, OnMasterSaveTable)
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_MESSAGE( WM_GRAPHNOTIFY, OnVideoStop )
	ON_WM_SHOWWINDOW()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP

	ON_NOTIFY( NM_USER_PROPERTY, 0, OnNotifyTooltip )
//	ON_COMMAND_RANGE( ID_MENU_GROUP, ID_MENU_GROUP + 99, OnDynamicGroupMenu )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraView drawing

static CMainFrame* GetMainWnd() 
{
	return static_cast<CMainFrame*>( AfxGetMainWnd() );
}

void CCameraView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
//	m_MenuManager.Install( , this );
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	BITMAPINFO* pbi;
	BYTE*		pbits;		
	CVVMapEditorDoc* pDoc = GetDocument();
	try
	{
		int nIndex = pDoc->IdToIndex( m_Number );

		long res = pDoc->m_VVMapTrans->GetMasterBitmap(nIndex, &pbi, &pbits );

		if(!res && pbits && pbi)
		{
			BITMAPINFO bi;
			memcpy(&bi, pbi, sizeof(BITMAPINFO));
			bi.bmiHeader.biClrImportant = 0;
			bi.bmiHeader.biClrUsed = 0;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biPlanes = 1;
			m_Background = std::unique_ptr<CBackground>(new CBitmapBackground(&bi, pbits) );
		}

		SetScroll( );
		GetParent()->SendMessage(WM_NCPAINT);

		if ( !::IsWindow(m_wndToolTip.GetSafeHwnd()) )
		{
			m_wndToolTip.Create(this);
			m_wndToolTip.On(TRUE);
			m_wndToolTip.SetOffset( 15,15 );
		}
	}
	catch(CommonException&)
	{

	}
}

void CCameraView::OnUpdateMasterSrs( CCmdUI* pCmdUI)
{

	pCmdUI->SetCheck( m_bSrs ? 1 : 0 );
}

void CCameraView::OnDraw(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	// TODO: add draw code here

//	pDC->Rectangle(20, 20, 200, 200);

	CRect rc;
	GetClientRect(&rc);

	Graphics	Native(pDC->GetSafeHdc());

	CSize size = m_Background->GetSize();
	
	CSize szScreen;
	szScreen.cx = MY_max( rc.Width(), size.cx );
	szScreen.cy = MY_max( rc.Height(), size.cy );

	Bitmap		BackBmp(szScreen.cx, szScreen.cy);
	Graphics	BackBuffer(&BackBmp);

	BackBuffer.Clear(Color::LightGray);

	CRectF rcBG(0,0,0,0);
	rcBG = CRectF( 0, 0, size.cx, size.cy);
	
	m_Background->Draw( BackBuffer, rcBG );

	if( m_bSrs )	DrawSRSPoints( BackBuffer, BackBmp);

	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent(m_Number))
	{

		//NOTENOTE: first draw polygon, they take more large area;
		DrawPolygons(BackBuffer);
		DrawHoryzon( BackBuffer	);
		DrawPoints(BackBuffer, rc);
		DrawExtents(BackBuffer);
	}


	CRect rcOutput = GetStartPoint();


	Native.DrawImage( &BackBmp, rcOutput.left, rcOutput.top);

	DrawGetPoints(pDC);
	DrawHeight(pDC);
}

CRect CCameraView::GetStartPoint()
{
	CRect rc;
	GetClientRect(&rc);
	if(!m_Background.get()) return CRect(0,0, 0, 0);
	CSize size = m_Background->GetSize();
	int X = rc.CenterPoint().x - size.cx/2;
	int Y = rc.CenterPoint().y - size.cy/2;
/*
	if( X  < 0) 
	{
		size.cx += X;
		X = 0;
	}
	if( Y  < 0) 
	{
		size.cy += Y;
		Y = 0;
	}*/
	return CRect( CPoint(X, Y), size) ;
}

void	CCameraView::DrawPoints(Graphics& gr, const CRect& rcClient)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() &&
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowCommonPoints) 
	{
		SolidBrush br( Color::Red);
		Pen pen(Color::Black, 1);
		int Index = pDoc->IdToIndex(m_Number);
		const PointArr_t& pt = pDoc->Points().m_Points[ Index ];
		int TotalPoint = 0;

		std::unique_ptr<CFilterCache> pFilterCache = pDoc->GetFilterCache();

		for( int i = 0 ;i < pt.size(); ++i)
		{
			if( pt[i].first && pFilterCache->IsPointValid( i, Index ) )
			{
				const PointF ptz = pt[i].second;
				DWORD dwColor = GetSettings().GetPointColor( i) ;
				br.SetColor(dwColor );
				gr.FillRectangle(&br, RectF( ptz.X - 1, ptz.Y - 1 ,  3,  3 ));
				gr.DrawRectangle(&pen,RectF( ptz.X - 2, ptz.Y - 2 ,  4,  4 ));

				if( pDoc->m_bShowPointNumber )
				{
					DrawPointGroup( gr, ptz, i, br );
				}
				++TotalPoint; 
			}
		}
		if( pDoc->m_bShowPointNumber  ) DrawPointNumber( gr, rcClient, TotalPoint );
	}
}

CRectF GetRect( PointF st , PointF end )
{
		PointF pt ( std::min( st.X, end.X ), std::min( st.Y, end.Y ) ) ;
		SizeF sz ( fabs( st.X -  end.X ), fabs( st.Y - end.Y ) ) ;
		return CRectF( RectF(pt, sz) );
}

void	CCameraView::DrawExtents(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	//Color cl(Color::Red);
	Color cl = GetSettings().GetProperty( prop_ExtentColor );
	Pen br(  cl, 1);


	if(	!(pDoc->IsCurrent() &&
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowExtents))
	{
		return;
	}

	const ExtentSelectedEditState* pExtSelEdit = 
		dynamic_cast<ExtentSelectedEditState*>( m_EditorState.m_Edit.get() );
	const ExtentNewEditState* pExtNewEdit = 
		dynamic_cast<ExtentNewEditState*>( m_EditorState.m_Edit.get() );

	ExtentMap_t::const_iterator itr = pDoc->Extents().begin() + pDoc->IdToIndex(m_Number);
	if(  itr != pDoc->Extents().end() )
	{
		const std::vector<CRectF>& pt = *itr;
		int z = pt.size();
		for( int i = 0; i < z; ++i)
		{
			if( pExtSelEdit && pExtSelEdit->GetSelectExtent() == i )
			{
				br.SetDashStyle( DashStyleDot );
				gr.DrawRectangle(&br, pt[i]);
				br.SetDashStyle(  DashStyleSolid);
			}
			else
				gr.DrawRectangle(&br, pt[i]);
		}
	}

	if( pExtNewEdit )
	{
		RectF rcf = pExtNewEdit->GetCurrentRect();
		br.SetDashStyle(  DashStyleDot);
		gr.DrawRectangle(&br, rcf );
	}
	/*
	if( pDoc->m_State == CVVMapEditorDoc::edit_extents && 
		m_EditorState.m_State == EditorState::EDIT )
	{	
		br.SetDashStyle(  DashStyleDot);

		if(m_EditorState.m_ExtNumber == -1)
			gr.DrawRectangle(&br, GetRect(m_EditorState.m_RectPoint[0], m_EditorState.m_RectPoint[1]) );
		else
		{
			
		}
	}*/
}

void	CCameraView::DrawPolygons(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	

//	if(!pDoc->IsCurrent( || P) return;
	int Index = pDoc->IdToIndex( m_Number );
	if( pDoc->m_bShowPolygons  && !pDoc->m_Polygons[ Index ].empty())
	{
		PolygonArr_t::const_iterator itr = pDoc->m_Polygons[ Index ].begin();
		for(int zzz = 0; itr != pDoc->m_Polygons[Index].end();++itr, ++zzz )
		{
			const Polygon_t& pt = *itr ;
			if (pt.empty())
				break;
			int PointNumber = pt.size();
//			const PointF* ptz = &pt[0];
			int z = pDoc->IndexToId( zzz );
			Color clr = GetSettings().GetMasterColor( z );
			Color cl = Color::MakeARGB( 100, clr.GetR(), clr.GetG(), clr.GetB() );
			SolidBrush br(cl );
			gr.FillPolygon( &br, &pt[0], pt.size() );
		}	
	}
}

void	CCameraView::DrawGetPoints(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowGetPoints)
	{
		long x = pDoc->m_GetPoint.second.x;
		long y = pDoc->m_GetPoint.second.y;
		long xto, yto;

		int Index = pDoc->IdToIndex(m_Number);
		if( Index == pDoc->m_GetPoint.first) return;
		long err = pDoc->m_VVMapTrans->GetXY( pDoc->m_GetPoint.first, x, y, Index, &xto, &yto  );
	
		CSize sizeCross (	GetSettings().GetProperty( prop_CrossExtent		) , 
							GetSettings().GetProperty( prop_CrossThickness	) );
		CCrossCache cache( CPoint(xto, yto), err == 0, sizeCross.cx/2, sizeCross.cy );
		m_CrossDC.Draw( pDC, cache );
	}
}

void	CCameraView::DrawGetPoint()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowGetPoints)
	{
		long x = pDoc->m_GetPoint.second.x;
		long y = pDoc->m_GetPoint.second.y;
		long xto, yto;

		int Index = pDoc->IdToIndex(m_Number);
		if( Index == pDoc->m_GetPoint.first) 
		{
			m_CrossDC.InvalidateCache(this);
			return;
		}
		long err = pDoc->m_VVMapTrans->GetXY( pDoc->m_GetPoint.first, x, y, Index, &xto, &yto  );

		CSize sizeCross (	GetSettings().GetProperty( prop_CrossExtent		) , 
							GetSettings().GetProperty( prop_CrossThickness	) );

		CCrossCache cache( CPoint(xto, yto), err == 0, sizeCross.cx/2, sizeCross.cy );
		m_StateCache.DrawInvert( this  );
		m_StateCache = CStateCache( CPoint(5,5), err, this );
		m_StateCache.Draw( this  );
		m_CrossDC.Draw( this, cache );
	}	
}

void	CCameraView::DrawSelectPoint(int nGroupNumber)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( m_Number ))
	{
		int Index = pDoc->IdToIndex(m_Number);

		std::unique_ptr<CFilterCache> pFilterCache = pDoc->GetFilterCache();
		const std::pair<bool, PointF>& ptz = pDoc->Points().m_Points[Index][nGroupNumber];

		if(ptz.first && pFilterCache->IsPointValid( nGroupNumber, Index )  )
		{
			CPoint pt (ptz.second.X, ptz.second.Y);
			CPointCache cache( pt );
			m_PointDC.Draw( this, cache );
		}
	}	
}

void	CCameraView::DrawHeight(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowHeights)
	{
		int Index = pDoc->IdToIndex(m_Number);
		if( Index != pDoc->m_HeightPoint.first) return;

		CPoint ptHeight = pDoc->m_HeightPoint.second;
		int x = ptHeight.x;
		int y = ptHeight.y;

		if( x < 0 || x > 352 ||
			y < 0 || y > 288) return;
		long lHeight = pDoc->m_ExtentTable[Index](x,y);

		CExtentCache cache( CRect(	ptHeight.x - lHeight/8 , ptHeight.y - lHeight,  
									ptHeight.x + lHeight/8 , ptHeight.y ) );
		m_ExtentDC.Draw(pDC, cache);
		
	}
}

void	CCameraView::DrawHeight()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_bShowHeights)
	{
		int Index = pDoc->IdToIndex(m_Number);
		if( Index != pDoc->m_HeightPoint.first) 
		{
			m_ExtentDC.InvalidateCache(this);
			return;
		}

		CPoint ptHeight = pDoc->m_HeightPoint.second;
		int x = ptHeight.x;
		int y = ptHeight.y;

		const Array<long>& Arr = pDoc->m_ExtentTable[Index];
		if( x < 0 || x >= Arr.GetX() ||
			y < 0 || y >= Arr.GetY() ) return;
		long lHeight = Arr(x,y);

		if(lHeight)
		{
			int a = 1;
		}

		CExtentCache cache( CRect(	ptHeight.x - lHeight/8 , ptHeight.y - lHeight,  
									ptHeight.x + lHeight/8 , ptHeight.y ) );
		m_ExtentDC.Draw(this, cache);
	}	
}

/////////////////////////////////////////////////////////////////////////////
// CCameraView diagnostics

#ifdef _DEBUG
void CCameraView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCameraView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

CVVMapEditorDoc* CCameraView::GetDocument()
{
	return (CVVMapEditorDoc*)m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CCameraView message handlers

BOOL CCameraView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CScrollView::OnEraseBkgnd(pDC);
}


void	CCameraView::SetScroll()
{
		CSize size = m_Background->GetSize();
		CRect rc;
		GetParent()->GetClientRect(&rc);
		CSize sz;
		sz.cx = MY_min( rc.Width(), size.cx );
		sz.cy = MY_min( rc.Height(), size.cy );
		rc = CRect( CPoint(0,0), size);
		CalcWindowRect  ( & rc, CWnd::adjustOutside);
		rc.NormalizeRect();
		SetWindowPos(0, 0, 0, size.cx + 3, size.cy +3, SWP_NOZORDER | SWP_NOMOVE);
		SetScrollSizes(MM_TEXT, size-CSize(5,5));
}

void	CCameraView::OnMasterImage()
{
	CString Filter;
	Filter.LoadString( IDS_FILTER_IMAGE );
	CFileDialogEx dlg ( TRUE, NULL, NULL,  0, Filter, this);
	if(dlg.DoModal() != IDOK) return;
	std::wstring str =  Helper::ConvertW( dlg.GetPathName() );

	try
	{
		m_Background = std::unique_ptr<CBackground> ( new CBitmapBackground( str ) );
		SetScroll();

	}
	catch(ImageException& ex)
	{
		AfxMessageBox( Convert(ex.what()), MB_OK | MB_ICONERROR );
	}
	ResetAllSettings();	
	Invalidate(FALSE);
}

void	CCameraView::OnMasterVideo()
{
	CString Filter;
	Filter.LoadString( IDS_FILTER_VIDEO);
	CFileDialogEx dlg ( TRUE, NULL, NULL,  0, Filter, this);
	if(dlg.DoModal() != IDOK) return;
	std::wstring str =  Helper::ConvertW( dlg.GetPathName() );

	try
	{
		m_Background = std::unique_ptr<CBackground> ( new CVideoBackground( this, str ) );
/*		CSize size = m_Background->GetSize();
//		SetScrollSizes(MM_TEXT, size - CSize(4,4));
		CRect rc;
		GetParent()->GetClientRect(&rc);
		CSize sz;
		sz.cx = min( rc.Width(), size.cx );
		sz.cy = min( rc.Height(), size.cy );
		SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOMOVE);
		SetScrollSizes(MM_TEXT, size - CSize(5,5));*/
		SetScroll();
//		((COXSizeControlBar*)GetParent())->Invalidate();
	}
	catch(VideoException& ex)
	{
		AfxMessageBox( Convert(ex.what()), MB_OK | MB_ICONERROR );
	}
	ResetAllSettings();	
	Invalidate(FALSE);
}

void	CCameraView::ResetAllSettings()
{
}


void	CCameraView::Resize(CSize size)
{
/*	if( dynamic_cast<CEmptyBackground*> ( m_Background.get() ) )
	{
		SetWindowPos(0, 0, 0, 352+3, 288+3, SWP_NOZORDER | SWP_NOMOVE);
	}
	else
	{
		CSize sz = m_Background->GetSize();
		sz.cx = min( sz.cx, size.cx);
		sz.cy = min( sz.cy, size.cy);
		SetWindowPos(0, 0, 0, sz.cx+3, sz.cy+3, SWP_NOZORDER | SWP_NOMOVE);
	}*/
	SetScroll();
}




void CCameraView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if(	pDoc->IsCurrent() && pDoc->IsCameraInCurrent( m_Number ))
	{
		int Index = pDoc->IdToIndex(m_Number);
		switch (pDoc->m_State) 
		{
		case  CVVMapEditorDoc::edit_points :
		{
			
			PointF ptf(point.x, point.y);

			m_GroupMenu.DestroyMenu();
			m_GroupMenu.CreatePopupMenu();
			pDoc->FillGroupCameraMenu(m_GroupMenu, Index);
			CPoint ptScr(point); 
			ClientToScreen(&ptScr);
			m_EditorState.m_Edit = std::unique_ptr<ItemEditState>( new PointEditState( PointF (point.x, point.y) ) );
			BOOL nID = m_GroupMenu.TrackPopupMenu( TPM_RETURNCMD|TPM_NONOTIFY , ptScr.x, ptScr.y, this );
			if( nID != 0)
			{
				OnDynamicGroupMenu( nID );
			}

			break;
		}
		case  CVVMapEditorDoc::edit_extents :
			{

				StartEdit();
				PointF ptf(point.x, point.y);
				std::pair<int, CVVMapEditorDoc::Part> PtIndex = pDoc->PtInExtent( Index, point );
				std::vector<CRectF>& Arr = pDoc->Extents()[Index];
				pDoc->Backup();
				if( PtIndex.first != -1 )
				{
					CRectF& rect = pDoc->Extents()[Index][PtIndex.first];
					m_EditorState.m_Edit = std::unique_ptr<ItemEditState>
							( new ExtentSelectedEditState( ptf, PtIndex.first, rect, PtIndex.second ) );
				}
				else
				{
					m_EditorState.m_Edit = std::unique_ptr<ItemEditState>
							( new ExtentNewEditState( ptf ) );
				}
				
				m_EditorState.m_State = EditorState::EDIT;
			}
			Invalidate();
			break;
		}
	}
	CScrollView::OnLButtonDown(nFlags, point);
}



BOOL CCameraView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
/*	cs.cx = 352+3;
	cs.cy = 288+3;
	cs.style &= ~(WS_THICKFRAME);*/
	return CScrollView::PreCreateWindow(cs);
}

void CCameraView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here

}


void	CCameraView::StartEdit()
{
	SetCapture();
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	ClipCursor(&rc);
}

void	CCameraView::EndEdit()
{
	ClipCursor(0);
	ReleaseCapture();
	m_EditorState.m_State = EditorState::NONE;
}


void CCameraView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	const int nRadius = 15;
	CRect rc( point, CSize(nRadius,nRadius) );
	rc.OffsetRect( -nRadius/2, -nRadius/2);
	if(	pDoc->IsCurrent() &&
		pDoc->IsCameraInCurrent( m_Number ) &&
		pDoc->m_State == CVVMapEditorDoc::edit_points) 
	{	
		int Index = pDoc->IdToIndex( m_Number );
		const PointArr_t& v = pDoc->Points().m_Points[Index];
		for( int i=0; i < v.size(); ++i  )
		{
			//PointF ptf()
			const PointF& ptz = v[i].second;
			if( rc.PtInRect( CPoint( ptz.X, ptz.Y) ) )
			{
				pDoc->Backup();

				m_EditorState.m_Edit = std::unique_ptr<ItemEditState>
					(new PointSelEditState( pDoc->Points().m_Points[Index][i].second ));
				m_EditorState.m_State = EditorState::EDIT;
				
//				pDoc->Backup();
				StartEdit();
				break;
			}
		}
		
	}
	else
	{
		CMenu* pMenu = GetMainWnd()->GetSubMenu(0);
		CPoint pt(point);
		ClientToScreen(&pt);
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, /*this*/this );
	}
	CScrollView::OnRButtonDown(nFlags, point);
}

void CCameraView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if( m_EditorState.m_State == EditorState::EDIT)
	{
		
		EndEdit();
	}
	CScrollView::OnRButtonUp(nFlags, point);
}

void CCameraView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_wndToolTip.Set(point , _T(""));
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	
	if( pDoc->IsCurrent() &&
		pDoc->IsCameraInCurrent(m_Number) &&
		m_EditorState.m_State == EditorState::EDIT )
	{
		int Index = pDoc->IdToIndex( m_Number );
		//TODO: from mouse to virtual
		PointF CurPoint(point.x, point.y);
	switch(pDoc->m_State)
	{
		
	case CVVMapEditorDoc::edit_points:
	{
/*		PointF CurPoint(point.x, point.y);
		PointF & pt = pDoc->Points().m_Points[Index][m_EditorState.m_Number].second;
		pt.X += CurPoint.X - m_EditorState.m_MousePoint.X ;
		pt.Y += CurPoint.Y - m_EditorState.m_MousePoint.Y ;
		m_EditorState.m_MousePoint = PointF (point.x, point.y);*/
		m_PointDC.InvalidateCache(this);
		m_EditorState.m_Edit->SetMousePoint( CurPoint );
		Invalidate();
	}
	break;
	case CVVMapEditorDoc::edit_extents:
	{
/*		PointF CurPoint(point.x, point.y);
		m_EditorState.m_RectPoint[1].X += CurPoint.X - m_EditorState.m_MousePoint.X ;
		m_EditorState.m_RectPoint[1].Y += CurPoint.Y - m_EditorState.m_MousePoint.Y ;
		if( m_EditorState.m_ExtNumber  != -1)
		{
			CRectF& rect = pDoc->Extents()[Index][m_EditorState.m_ExtNumber];
			rect = GetRect( m_EditorState.m_RectPoint[0], m_EditorState.m_RectPoint[1]) ;
		}	
		m_EditorState.m_MousePoint = CurPoint;*/
		m_EditorState.m_Edit->SetMousePoint( CurPoint );
		Invalidate();
		break;
	}
	}

	}
	
	if( pDoc->IsCurrent()			&& 
		pDoc->IsCameraInCurrent(m_Number) 
		// Это позволяет видеть преобразование при редактировании
		/*&&
		pDoc->m_State == CVVMapEditorDoc::usual &&		
		m_EditorState.m_State == EditorState::NONE*/ &&
		pDoc->m_bShowGetPoints)
	{
		PointF CurPoint(point.x, point.y);
		int Index = pDoc->IdToIndex( m_Number);
		if( pDoc->m_bShowGetPoints  ) pDoc->m_GetPoint = std::make_pair( Index, point );

		pDoc->DrawGetPoint();


		SetTimer( IDT_GetPoint, 100, 0 );

	}

	if( pDoc->IsCurrent()			&& 
		pDoc->IsCameraInCurrent(m_Number)  &&
		pDoc->m_bShowHeights)
	{
		PointF CurPoint(point.x, point.y);
		int Index = pDoc->IdToIndex( m_Number);
		pDoc->m_HeightPoint = std::make_pair( Index, point );

		pDoc->DrawHeight();


		SetTimer( IDT_HeightPoint, 100, 0 );

	}
	
	if( pDoc->IsCurrent()			&& 
		pDoc->IsCameraInCurrent(m_Number) 
		// Это позволяет видеть преобразование при редактировании
		/*&&
		pDoc->m_State == CVVMapEditorDoc::usual &&
		m_EditorState.m_State == EditorState::NONE */)
	{
		int Index = pDoc->IdToIndex( m_Number );
		const PointArr_t& Arr = pDoc->Points().m_Points[ Index ];
		bool set = false;
		for( int i=0;  i < Arr.size(); ++i)
		{
			if( Arr[i].first)
			{
				CPoint ptStart( Arr[i].second.X, Arr[i].second.Y  );
				CRect rc( ptStart, CSize(10,10) );
				rc.OffsetRect(-5,-5);
				if(rc.PtInRect( point ))
				{
					m_nSelectGroup = i;
					SetTimer( IDT_SelectPoint, 300, 0 );
					set = true;
					break;
				}
			}
		}
		if( !set )
		{
			KillTimer( IDT_SelectPoint );
			pDoc->InvalidateSelectPoint(  );
		}
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}

BOOL CCameraView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if(	pDoc->IsCurrent() && 	pDoc->IsCameraInCurrent( m_Number ))
	{
		const ExtentSelectedEditState* pExtSelEdit = 
			dynamic_cast<ExtentSelectedEditState* > ( m_EditorState.m_Edit.get() );
		switch(  pDoc->m_State)
		{
			case CVVMapEditorDoc::edit_points:
			SetCursor( m_hCross );
				return TRUE;				
			case CVVMapEditorDoc::edit_extents:
/*				if( pExtSelEdit && m_EditorState.m_State == EditorState::EDIT)
				{
					SetCursor( m_hArrow[ pExtSelEdit->GetConstraint() ] );
				}
				else*/
				CPoint ptCursor;
				GetCursorPos(&ptCursor);
				ScreenToClient( &ptCursor );
				int nIndex = pDoc->IdToIndex( m_Number );
				std::pair<int, CVVMapEditorDoc::Part> Cur = pDoc->PtInExtent(nIndex, ptCursor);
				if( Cur.first != -1 )
					SetCursor( m_hArrow[ Cur.second] );
				else
					SetCursor( m_hCross );
				return TRUE;
		}
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

void CCameraView::OnMasterSrs() 
{
	// TODO: Add your command handler code here
	m_bSrs = ! m_bSrs;
	
}


void CCameraView::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu )
{
//	CScrollView::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
	UpdateWindow();
	CScrollView::OnInitMenuPopup(  pPopupMenu, nIndex, bSysMenu);
	CNewMenu::OnInitMenuPopup(  GetSafeHwnd(), pPopupMenu, nIndex, bSysMenu);
/*
	if ( pPopupMenu->GetSafeHmenu() == GetMainWnd()->GetSubMenu(0)->GetSafeHmenu() )
	pPopupMenu->CheckMenuItem( ID_MASTER_SRS,  (m_bSrs?  MF_CHECKED: MF_UNCHECKED ) | MF_BYCOMMAND );*/

}

static DWORD GetBmpLenght(LPBITMAP pBITMAP)
{
	if(pBITMAP->bmWidth <= 0 || pBITMAP->bmHeight <= 0)
		return 0;//to left bitmap

	return ((pBITMAP->bmWidth * pBITMAP->bmPlanes * pBITMAP->bmBitsPixel + 31) & ~31) /8 * pBITMAP->bmHeight;
}

#if 0
using namespace Elvees::Win32;
#endif

void	CCameraView::DrawSRSPoints( Graphics& gr, Bitmap& bmp )
{
#ifdef USE_SRS
	HBITMAP hBitmap;
	CImage* pImage = 0;
	bmp.GetHBITMAP( Color::Black, &hBitmap );
	BITMAP bm;
	GetObject( hBitmap, sizeof(BITMAP), & bm );


	BITMAPINFO	bi;
	memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    bi.bmiHeader.biWidth = bm.bmWidth;
    bi.bmiHeader.biHeight = bm.bmHeight;
    bi.bmiHeader.biPlanes = bm.bmPlanes;
    bi.bmiHeader.biBitCount = bm.bmBitsPixel;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = GetBmpLenght( &bm );
	
	CPoint objPosition;
	SolidBrush br( Color( 255, 255,255, 0 )  );
	Pen brRect( Color( 128, 255,255, 0 ) ,1  );
	bool res = CreateImageIndirect( &pImage, &bi, (BYTE*)bm.bmBits  );
	if( pImage )
	{
		ObjectReg *obj; 
		m_pAnalyzer->HandleBmp( pImage );
		long NumberObjects;
		if (!m_pAnalyzer->GetObjects(&obj,&NumberObjects))
		{
			m_Rects.clear();
			m_Rects.reserve(NumberObjects);
			for (int index=0; index<NumberObjects; index++) 
			{
				switch(obj[index].status) 
				{
					case 1: 
					case 2:
						//	if (!obj[index].suspeciousObject) 
							{
								objPosition.x=(obj[index].xleft+ obj[index]. xright)/2;
								objPosition.y=obj[index]. ydown;
							}
							CRect rcObject(obj[index].rect );
							gr.FillEllipse(&br, RectF( objPosition.x - 2, objPosition.y - 2 ,  5,  5 ));
							gr.DrawRectangle(&brRect, RectF( rcObject.left, rcObject.top  , rcObject.Width(),  rcObject.Height() ));
							m_Rects.push_back( rcObject );

				}	
			}
		}
		m_pAnalyzer->DeleteObjects (obj);	
		pImage->Release();
	}

	DeleteObject(hBitmap);
#endif
}

void CCameraView::OnMasterCamera() 
{
	// TODO: Add your command handler code here
	try
	{
		CRealtimeBackground* pBg = new CRealtimeBackground(  this,/* CamID*/ m_Number ) ;
		m_Background = std::unique_ptr<CBackground> ( pBg );
		ResetAllSettings();
//		m_PlayerSettings.State = PlayerSettings::play;
	}
	catch(std::exception& ex)
	{
		AfxMessageBox( Convert(ex.what()), MB_OK | MB_ICONERROR );
		ResetAllSettings();
//		m_Background = std::auto_ptr<CBackground> ( new CEmptyBackground( )  );
		Invalidate(FALSE);
	}	
}

void	AdjustForMinExtentSize( RectF& rcf )
{
	const double n_MinExtentX = 5.0f;
	const double n_MinExtentY = 5.0f;

	rcf.Width = std::max<double>( n_MinExtentX, rcf.Width  );
	rcf.Height = std::max<double>( n_MinExtentY, rcf.Height  );
}

void CCameraView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default


	CVVMapEditorDoc* pDoc = GetDocument();
	if(	pDoc->IsCurrent())
	switch (pDoc->m_State) 
	{
	case  CVVMapEditorDoc::edit_extents :
		{
			PointF ptf(point.x, point.y);
			int nIndex = pDoc->IdToIndex(m_Number);
			const ExtentNewEditState* pExtNewEdit = 
				dynamic_cast<ExtentNewEditState*> ( m_EditorState.m_Edit.get() );
			const ExtentSelectedEditState* pExtSelEdit = 
				dynamic_cast<ExtentSelectedEditState*> ( m_EditorState.m_Edit.get() );
			if( pExtNewEdit )
			{
	//			pDoc->Backup();
				CRectF rcf = pExtNewEdit->GetCurrentRect();
				//NOTENOTE: min is 5x5
				AdjustForMinExtentSize( rcf );
				// need to delegate for CVVMapEditorDoc
				pDoc->Extents()[nIndex].push_back( rcf );
				int Number = pDoc->Extents()[nIndex].size() - 1;
				m_EditorState.m_Edit = std::unique_ptr<ItemEditState>
					(new ExtentSelectedEditState( ptf, Number, RectF(), 0 ) );
			}
			else if( pExtSelEdit )
			{
				int nExtNumber = pExtSelEdit->GetSelectExtent();
				RectF& rcf = pDoc->Extents()[nIndex][nExtNumber];
				AdjustForMinExtentSize( rcf );
			}
		}
		m_EditorState.m_State = EditorState::NONE;
		EndEdit();
		Invalidate();
		break;
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void	CCameraView::Stop()
{
	CControlBackground* cbg = dynamic_cast<CControlBackground*>( m_Background.get() );
	if(cbg)
	{
		cbg->Stop();
	}
}

void	CCameraView::Play()
{
	CControlBackground* cbg = dynamic_cast<CControlBackground*>( m_Background.get() );
	if(cbg)
	{
		cbg->Play();
	}
}

void	CCameraView::Pause()
{
	CVideoBackground* vbg = dynamic_cast<CVideoBackground*>( m_Background.get() );
	if(vbg)
	{
		vbg->Pause();
	}
}

void CCameraView::OnMasterEmtpy() 
{
	// TODO: Add your command handler code here
	m_Background = std::unique_ptr<CBackground>(new CEmptyBackground);
	Invalidate();
}

void CCameraView::OnMasterOpenTable() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = GetDocument();
	CString strFilter; strFilter.LoadString( IDS_VMT_FILTER ) ;
	CFileDialogEx dlgFile( TRUE, _T("*.vmt"), 0, OFN_OVERWRITEPROMPT, strFilter );
	if( dlgFile.DoModal() == IDOK )
	{
		VVError res(pDoc->m_VVMapTrans);
		CFile file( dlgFile.GetFileName(), CFile::modeRead );
		std::vector<BYTE> Content;
		long Length = file.GetLength();
		Content.resize(Length);
		file.Read( &Content[0], Length );
		long Index = pDoc->IdToIndex(m_Number);
		void * z = &Content[0];
		z = z? z : (void*)1; 
		try
		{
			res = pDoc->m_VVMapTrans->LoadExtentTable(Index, z , Length);
		}
		catch(VVMapTransException& ex)
		{	
			AfxMessageBox( Convert(ex.what()));
		}
	}	
}

void CCameraView::OnMasterSaveTable() 
{
	// TODO: Add your command handler code here
	CVVMapEditorDoc* pDoc = GetDocument();
	CString strFilter; strFilter.LoadString( IDS_VMT_FILTER ) ;
	CFileDialogShortEx dlgFile( FALSE, _T("*.vmt"), 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter );
	dlgFile.m_ofn.Flags |= OFN_EXPLORER | OFN_ENABLETEMPLATE;
	dlgFile.m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD_SHORT);
	if( dlgFile.DoModal() == IDOK )
	{
		VVError res(pDoc->m_VVMapTrans);
		long Index = pDoc->IdToIndex(m_Number);
		BYTE* pByte;
		long Length;
		try
		{
			res =	dlgFile.m_bShort?
					pDoc->m_VVMapTrans->SaveExtentTableShort(Index, (void**)&pByte, &Length):
					pDoc->m_VVMapTrans->SaveExtentTable( Index, (void**)&pByte, &Length );
					CFile file( dlgFile.GetPathName(), CFile::modeWrite|CFile::modeCreate  );
					file.Write( pByte, Length );
		}
		catch(VVMapTransException& ex)
		{	
			AfxMessageBox( Convert(ex.what()));
		}
	}
}


void CCameraView::OnMasterSetid() 
{
	// TODO: Add your command handler code here
	std::vector<int> Ids;
	CVVMapEditorDoc* pDoc = GetDocument();
/*	CMainFrame::WndMap_t::const_iterator itr = GetMainWnd()->m_Windows.begin();
	for( ;itr != GetMainWnd()->m_Windows.end(); ++itr )
	{
		Ids.push_back( itr->first );
	}*/

	Ids = GetMainWnd()->GetIds();
	
	CSetIDDialog dlg(this, Ids);
	if( dlg.DoModal() == IDOK)
	{
		pDoc->Renumber(Ids, dlg.m_Result);
		GetMainWnd()->Renumber(Ids, dlg.m_Result);
		pDoc->UpdateAllViews(0);
	}
}

void CCameraView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	
	CVVMapEditorDoc* pDoc = GetDocument();
	// TODO: Add your message handler code here
	if( m_EditorState.m_State == EditorState::EDIT && 
		pDoc->m_State == CVVMapEditorDoc::edit_extents )
	{
		int nIndex = pDoc->IdToIndex(m_Number);
		const ExtentNewEditState* pExtSelEdit = 
			dynamic_cast<ExtentNewEditState*> ( m_EditorState.m_Edit.get() );
		if( pExtSelEdit )
		{
			pDoc->Backup();
			CRectF rcf = pExtSelEdit->GetCurrentRect();
			//NOTENOTE: min is 5x5
			if (rcf.Width < 5) rcf.Width = 5;
			if (rcf.Height < 5) rcf.Height = 5;
			pDoc->Extents()[nIndex].push_back( rcf );
			int Number = pDoc->Extents()[nIndex].size() - 1;
			m_EditorState.m_Edit = std::unique_ptr<ItemEditState>
				(new ExtentSelectedEditState( PointF(), Number, RectF(), 0 ) );
		}		
	}
	else if(	m_EditorState.m_State == EditorState::EDIT && 
				pDoc->m_State == CVVMapEditorDoc::edit_points )
	{
			m_EditorState.m_Edit = std::unique_ptr<ItemEditState>();
	}
	m_EditorState.m_State = EditorState::NONE;
	EndEdit();
	Invalidate();
}

void CCameraView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();

	const ExtentSelectedEditState* pExtSelEdit = 
		dynamic_cast<ExtentSelectedEditState*> ( m_EditorState.m_Edit.get() );
		
	switch( nChar )
	{
	case VK_DELETE:
		if( pDoc->IsCameraInCurrent(m_Number) &&
			pDoc->m_State == CVVMapEditorDoc::edit_extents && 
			pExtSelEdit )
		{	
			int n = pDoc->IdToIndex(m_Number);
			pDoc->Backup();
			pDoc->Extents()[n].erase( pDoc->Extents()[n].begin() + pExtSelEdit->GetSelectExtent() );
			m_EditorState.m_Edit = std::unique_ptr<ItemEditState>();
			Invalidate();
		}
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CCameraView::OnDynamicGroupMenu( UINT nID )
{
	int nGroup = nID - ID_MENU_GROUP;


	CVVMapEditorDoc* pDoc = GetDocument();
	int OwnIndex = pDoc->IdToIndex(m_Number);

	//FIXME: HACK!!!
	// this means we process only 100 groups of points!!!
	bool bNew = (nGroup == 0);
	
	//NOTENOTE: 0 is the new group
	--nGroup ;

	//PointF ptf = m_EditorState.m_MousePoint;
	const PointEditState* pPointEditState = 
			dynamic_cast<PointEditState*> ( m_EditorState.m_Edit.get() );
	if( pPointEditState )
	{
		PointF ptf = pPointEditState->GetMousePoint();
		pDoc->AddPointToGroup( bNew? -1 : nGroup, OwnIndex, ptf );
		GetMainWnd()->GetPointTree()->Refresh();	
	}		
}

void CCameraView::OnTimer(UINT nIDEvent)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( nIDEvent == IDT_GetPoint )
	{
		KillTimer( IDT_GetPoint );

		CRect rc;
		CPoint pt;
		GetCursorPos( &pt );
		GetClientRect( &rc );
		ScreenToClient(&pt);
		if( !rc.PtInRect( pt ) )
		{
			m_CrossDC.InvalidateCache(this);
		}
	} 
	else if( nIDEvent == IDT_SelectPoint )
	{
		CRect rc;
		CPoint pt;
		GetCursorPos( &pt );
		GetClientRect( &rc );
		ScreenToClient(&pt);
		if( !rc.PtInRect( pt ) )
		{
			KillTimer( IDT_SelectPoint );
			m_PointDC.InvalidateCache(this);
		}
		else
		{
			pDoc->DrawSelectPoint(m_nSelectGroup);
		}
	}
	else if( nIDEvent == IDT_HeightPoint )
	{
		KillTimer( IDT_HeightPoint );

		CRect rc;
		CPoint pt;
		GetCursorPos( &pt );
		GetClientRect( &rc );
		ScreenToClient(&pt);
		if( !rc.PtInRect( pt ) )
		{
			pDoc->InvalidateHeight();
		}
	}

	CScrollView::OnTimer(nIDEvent);
}

LRESULT CCameraView::OnVideoStop(WPARAM wParam, LPARAM lParam)
{
	CVideoBackground* pVideo = dynamic_cast< CVideoBackground* >( m_Background.get() );
	if(pVideo) pVideo->ProcessMessage();	
	return 0;
}

void	CCameraView::DrawPointGroup( Graphics& gr, PointF point, int Number, Brush& br)
{
	DrawHelper::DrawPointGroup(gr, point, Number, br);
}

void	CCameraView::DrawPointNumber( Graphics& gr, const CRect& rcClient, int Number)
{
	//NOTENOTE: offset from topleft corner
	const int c_nOffsetFromCornerX = 20; // 20px
	const int c_nOffsetFromCornerY = 20; // 20px
	SolidBrush br( GetSettings().GetProperty( prop_PointNumberColor ) );
	PointF point( rcClient.Width() - c_nOffsetFromCornerX, c_nOffsetFromCornerY );
	DrawHelper::DrawPointNumber( gr, point, Number, br);
}

void CCameraView::OnNotifyTooltip(NMHDR * pNMHDR, LRESULT * result)
{
    *result = 0;
	if( pNMHDR->hwndFrom	== m_wndToolTip.GetSafeHwnd() )
	{
		PropMap_t	prop;
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient( &point );

		int nIndex = FindCorrPointOnMap(point);
		if(nIndex != -1 )
		{
			CVVMapEditorDoc* pDoc = GetDocument();
			//const PointArr_t& Arr = pDoc->Points().m_MapPoints;

			std::vector<int> Masters = pDoc->FindWhereCorrPointExist(nIndex);
			CString strName;
			for( int i=0; i < Masters.size(); ++i)
			{
				const int c_nMapID = -1;

				if( Masters[i] == c_nMapID) 
				{
					strName.LoadString( IDS_MAP );
				}
				else
				{
					int z = pDoc->IndexToId( Masters[ i ] );
					strName.Format( _T("%d"), z );
				}
				prop.push_back( std::make_pair( strName, CString() ) );
			}
			m_wndToolTip.SetProperty( prop );
			*result = 1;
		}
	}
}

CPoint MapPointToMousePoint( PointF ptf )
{
	return CPoint ( ptf.X, ptf.Y );
}

int		CCameraView::FindCorrPointOnMap( CPoint point )
{
	CVVMapEditorDoc* pDoc = GetDocument();

	if( !pDoc->m_bShowCommonPoints ) return -1;
	int nIndex = pDoc->IdToIndex( m_Number );
	const PointArr_t& Arr = pDoc->Points().m_Points[ nIndex ];
	for( int i=0;  i < Arr.size(); ++i)
	{
		if( Arr[i].first)
		{
			CPoint ptStart = MapPointToMousePoint( Arr[i].second );
			CRect rc( ptStart, CSize(10,10) );
			rc.OffsetRect(-5,-5);
			if(rc.PtInRect( point ))
			{
				return i;
			}
		}
	}
	return -1;
}

//TODO: remove this
void CCameraView::OnShowWindow( BOOL bShow, UINT nStatus )
{
//	CVVMapEditorDoc*	pDoc = GetDocument();
	CScrollView::OnShowWindow(bShow, nStatus);
}

void	CCameraView::DrawHoryzon( Graphics& gr )
{
	CVVMapEditorDoc*	pDoc = GetDocument();

	int		nMasterIndex = pDoc->IdToIndex( m_Number );
	Color	clHoriz = GetSettings().GetMasterColor( m_Number );
	Pen		penHoriz( clHoriz, 2);

	const Polygon_t& Pol = pDoc->GetHoryzon( nMasterIndex );
	if (Pol.empty())
		return;
	const PointF* pptfStart = &Pol[0];
	gr.DrawLines( &penHoriz, pptfStart, Pol.size() );
}

void	CCameraView::ExtentSelectedEditState::SetMousePoint(PointF ptf)
{
	double dX = ptf.X - m_ptfOldMouse.X;
	double dY = ptf.Y - m_ptfOldMouse.Y;
	switch ( m_EditConstraint )
	{
	case CVVMapEditorDoc::top :
	case CVVMapEditorDoc::bottom:
		m_ptfDynamic.Y += dY;
		break;
	case CVVMapEditorDoc::left :
	case CVVMapEditorDoc::right:
		m_ptfDynamic.X += dX;
		break;
	case CVVMapEditorDoc::topleft :
	case CVVMapEditorDoc::topright :
	case CVVMapEditorDoc::bottomleft :
	case CVVMapEditorDoc::bottomright :
		m_ptfDynamic.X += dX;
		m_ptfDynamic.Y += dY;
		break;
	default:;
	}
	ItemEditState::SetMousePoint( ptf );
	UpdateRect();
}

RectF  CCameraView::ExtentNewEditState::GetCurrentRect() const
{
	double	fLeft = std::min( m_ptfStartMouse.X, m_ptfOldMouse.X ),
			fTop = std::min( m_ptfStartMouse.Y, m_ptfOldMouse.Y ),
			fRight = std::max( m_ptfStartMouse.X, m_ptfOldMouse.X ),
			fBottom = std::max( m_ptfStartMouse.Y, m_ptfOldMouse.Y );
	return RectF( fLeft, fTop, fRight - fLeft, fBottom - fTop );
}

CCameraView::ExtentSelectedEditState::ExtentSelectedEditState
			( PointF ptf, int Number, RectF& EditRect, int Constraint):
				ExtentEditState(ptf),
				m_ExtNumber(Number), m_EditRect(EditRect), m_EditConstraint(Constraint) 
{
	switch( Constraint )
	{
	case CVVMapEditorDoc::top:
	case CVVMapEditorDoc::topleft:
	case CVVMapEditorDoc::left:
		m_ptfStatic		= PointF( EditRect.X+EditRect.Width, EditRect.Y + EditRect.Height );
		m_ptfDynamic	= PointF( EditRect.X, EditRect.Y );
		break;
	case CVVMapEditorDoc::bottom:
	case CVVMapEditorDoc::bottomright:
	case CVVMapEditorDoc::right:
		m_ptfStatic		= PointF( EditRect.X, EditRect.Y  );
		m_ptfDynamic	= PointF( EditRect.X+EditRect.Width, EditRect.Y + EditRect.Height);
		break;		
	case CVVMapEditorDoc::topright:
		m_ptfStatic		= PointF( EditRect.X, EditRect.Y + EditRect.Height  );
		m_ptfDynamic	= PointF( EditRect.X+EditRect.Width, EditRect.Y);
		break;	
	case CVVMapEditorDoc::bottomleft:
		m_ptfDynamic	= PointF( EditRect.X, EditRect.Y + EditRect.Height  );
		m_ptfStatic		= PointF( EditRect.X+EditRect.Width, EditRect.Y);
		break;				
	}
}

void	CCameraView::ExtentSelectedEditState::UpdateRect()
{
	double	fLeft	= std::min( m_ptfStatic.X, m_ptfDynamic.X ),
			fTop	= std::min( m_ptfStatic.Y, m_ptfDynamic.Y ),
			fWidth	= abs( m_ptfStatic.X - m_ptfDynamic.X ),
			fHeight	= abs( m_ptfStatic.Y - m_ptfDynamic.Y );
	m_EditRect = RectF( fLeft, fTop, fWidth, fHeight );
}

void CCameraView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// release pointer
	m_EditorState.m_Edit = std::unique_ptr<ItemEditState>();
/*	if( m_EditorState.State == EditorState::EDIT )
	{
		Canc
	}*/
	m_EditorState.m_State = EditorState::NONE;
	EndEdit();
	Invalidate();
	CScrollView::OnUpdate( pSender, lHint, pHint );
}

//void	SetMousePoint(PointF ptf);