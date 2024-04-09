// VVMapEditorView.cpp : implementation of the CVVMapEditorView class
//

#include "stdafx.h"
#include "VVMapEditor.h"

#include "VVMapEditorDoc.h"
#include "VVMapEditorView.h"

#include "MainFrm.h"
#include "Helper.h"

#include "Zoom.h"

#include "CreateCADlg.h"
#include "FileDialogEx.h"
#include "DrawHelper.h"

#include "ImageBackground.h"
#include "EmptyBackground.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int IDT_GetPoint		= 1;
const int IDT_SelectPoint	= 2;
const int IDT_AnchorPoint	= 3;
const int IDT_RefPoint		= 4;
const int IDT_MapAbsCoords	= 5;

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView

IMPLEMENT_DYNCREATE(CVVMapEditorView, CScrollView)

BEGIN_MESSAGE_MAP(CVVMapEditorView, CScrollView)
	//{{AFX_MSG_MAP(CVVMapEditorView)
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MAP_IMAGE, OnMapImage)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_MAP_EXPORT, OnMapExport)
	ON_UPDATE_COMMAND_UI(ID_MAP_EXPORT, OnUpdateMapExport)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
//	ON_COMMAND_RANGE( ID_MENU_GROUP, ID_MENU_GROUP + 99, OnDynamicGroupMenu )
	ON_COMMAND_RANGE( ID_MENU_ANCHOR, ID_MENU_ANCHOR + 99, OnDynamicAnchorMenu )
	ON_NOTIFY( NM_USER_PROPERTY, 0, OnNotifyTooltip )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView construction/destruction

CVVMapEditorView::CVVMapEditorView():
	m_hCross( AfxGetApp()->LoadCursor( IDC_CROSS2 ) ),
	m_Scale(1.0f)
{
	// TODO: add construction code here

}

CVVMapEditorView::~CVVMapEditorView()
{
}

BOOL CVVMapEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
/*	cs.cx = 352;
	cs.cy = 288;*/
	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView drawing

void CVVMapEditorView::OnDraw(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CRect rc;
	GetClientRect(&rc);

//	pDC->FillSolidRect( &rc, RGB(0xD3, 0xD3, 0xD3));
//	return;

	CSize size = m_Background.get() ? m_Background->GetSize() : CSize(0,0);
	size.cx *= m_Scale;
	size.cy *= m_Scale;

	size.cx = max( rc.Width(), size.cx );
	size.cy = max( rc.Height(), size.cy );

	Graphics	Native(pDC->GetSafeHdc());
	Bitmap		BackBmp(size.cx, size.cy);
	Graphics	BackBuffer(&BackBmp);

	BackBuffer.Clear(Color::LightGray);

	CRectF rcBG(0,0,0,0);

	if( m_Background.get() )
	{
		CSize size = m_Background->GetSize();
		CPoint st = GetStartPoint();
		size.cx *= m_Scale;
		size.cy *= m_Scale;
		CRectF rcBG( st.x, st.y, size.cx, size.cy );
		m_Background->Draw( BackBuffer, rcBG);
		
		//NOTENOTE: first draw polygon ( larger than points)
		////TODO: remove matrix translation
		Matrix	AffineMatrix( m_Scale, 0.0f, 0.0f, m_Scale, st.x, st.y );
		Matrix  Identity;
		Identity.Reset();
		BackBuffer.SetTransform( &AffineMatrix);
		DrawPolygons(BackBuffer);
		BackBuffer.SetTransform( &Identity);
		DrawPoints(BackBuffer);
		BackBuffer.SetTransform( &AffineMatrix);
		DrawIZ(BackBuffer);
		DrawMapOrigin(BackBuffer);
		DrawAnchorPoints(BackBuffer);
		DrawRefPoints	(BackBuffer);
	}


	Native.DrawImage( &BackBmp, 0, 0, size.cx, size.cy );
	DrawGetPoints(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView printing

BOOL CVVMapEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVVMapEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVVMapEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView diagnostics

#ifdef _DEBUG
void CVVMapEditorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CVVMapEditorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CVVMapEditorDoc* CVVMapEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVVMapEditorDoc)));
	return (CVVMapEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVVMapEditorView message handlers


CMainFrame* GetMainWnd() 
{
	return static_cast<CMainFrame*>( AfxGetMainWnd() );
}

int CVVMapEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;	
	// TODO: Add your specialized creation code here
	return 0;
}

void CVVMapEditorView::OnMapImage() 
{
	// TODO: Add your command handler code here
	CString Filter;
	Filter.LoadString( IDS_FILTER_IMAGE );
	CFileDialogEx dlg ( TRUE, NULL, NULL,  0, Filter, this);
	if(dlg.DoModal() != IDOK) return;
	std::wstring str =  Helper::ConvertW( dlg.GetPathName() );

	CFile fileImage( dlg.GetPathName(), CFile::modeRead|CFile::shareDenyWrite );
	if( fileImage.m_hFile == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox( IDS_EXC_INVALID_IMAGE_FILE, MB_OK | MB_ICONERROR );
		return;
	}

	CVVMapEditorDoc* pDoc = GetDocument();

	size_t nSize = (size_t)fileImage.GetLength();
	std::vector<BYTE> BinArr( nSize );
	try
	{
		fileImage.Read( &BinArr[0], nSize );
		m_Background = std::unique_ptr<CBackground> ( new CImageBackground( &BinArr[0], nSize) );
		CSize size = m_Background->GetSize();
		SetScrollSizes( MM_TEXT, size );
		Invalidate();

		pDoc->SetMapCache( BinArr );
	}
	catch(CException*)
	{
		m_Background = std::unique_ptr<CBackground> ( new CEmptyBackground );
		AfxMessageBox( IDS_EXC_INVALID_IMAGE_FILE, MB_OK | MB_ICONERROR );
	}

#if 0
	try
	{
		//NOTENOTE: bugfix 10/14/2004
		m_Background = std::auto_ptr<CPicBackground> ( new CBitmapBackground( str ) );
		CSize size = m_Background->GetSize();
		SetScrollSizes( MM_TEXT, size );
		Invalidate();
	}
	catch(ImageException& ex)
	{
		AfxMessageBox( Helper::Convert(ex.what()), MB_OK | MB_ICONERROR );
	}
#endif

}

BOOL CVVMapEditorView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void CVVMapEditorView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	SetScrollSizes( MM_TEXT, CSize(0,0) );
	//m_Background = std::auto_ptr<CPic>();	
	CVVMapEditorDoc* pDoc = GetDocument();


	GetMainWnd()->GetPointTree()->Refresh();
	//NOTENOTE: first delete all items in quality bar
	GetMainWnd()->GetQualityBar()->Refresh( std::vector<MergeQuality>());

	SetTimer( IDT_MapAbsCoords, 100, 0 );

	GetMapInfo();
	if ( !::IsWindow(m_wndToolTip.GetSafeHwnd()) )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.On(TRUE);
		m_wndToolTip.SetOffset( 15,15 );
	}
}

void CVVMapEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if( !pDoc->IsCurrent() || !	pDoc->m_bMap) return;

	CPoint ptMouse = point  + GetScrollPosition();
	switch (pDoc->m_State)
	{
	
	case CVVMapEditorDoc::edit_map :
	{
		//FIXME: Add correct stuff
		Invalidate();
		m_EditorState.m_State = EditorState::EDIT;
		m_EditorState.m_Point = MousePointToMapPoint( point );
		StartEdit();
		break;
	}
	case CVVMapEditorDoc::edit_map_origin :
	{
//		Invalidate();
		PointF ptf = MousePointToMapPoint( point );
		pDoc->SetMapOrigin( ptf );
		Invalidate();
		break;
	}
	case CVVMapEditorDoc::edit_points :
	{
		PointF ptf = MousePointToMapPoint( point );

		m_EditorState.m_Point = ptf;
		m_GroupMenu.DestroyMenu();
		m_GroupMenu.CreatePopupMenu();
		pDoc->FillGroupMapMenu(m_GroupMenu);
		CPoint ptScr(point); 
		ClientToScreen(&ptScr);
		BOOL nID = m_GroupMenu.TrackPopupMenu( TPM_RETURNCMD | TPM_NONOTIFY, ptScr.x, ptScr.y, this );
		if( nID != 0 )
		{
			OnDynamicGroupMenu( nID )	;
		}
		//m_GroupMenu.TrackPopupMenuEx()
		break;
	}
	case CVVMapEditorDoc::edit_iz:
	{
		PointF ptf = MousePointToMapPoint( point );

		int nActiveIZ = -1;
		if( m_EditorState.m_State == EditorState::NONE  &&
			(nActiveIZ = pDoc->FindActiveIZ(ptf)) != -1)
		{
			pDoc->SetActiveIZ( nActiveIZ);
			Invalidate();
		}
		else
		{
			m_EditorState.m_State = EditorState::EDIT;
			if( !m_EditorState.m_CurrentPoly.get())
			{
				StartEditIZ();
				m_EditorState.m_CurrentPoly = std::unique_ptr<CFigPolyline>( new CFigPolyline(ptf) );
			}
			else
			{
				Zoom zoom(m_Scale, m_Scale); 
				zoom.Offset = GetStartPoint() - GetScrollPosition();
				m_EditorState.m_CurrentPoly->InvalidateEdit( this, zoom );
				m_EditorState.m_CurrentPoly->LButtonDown(ptf);
			}
			Zoom zoom(m_Scale, m_Scale); 
			zoom.Offset = GetStartPoint() - GetScrollPosition();
			m_EditorState.m_CurrentPoly->DrawEdit( this, zoom );
		}
		break;
	}
	case CVVMapEditorDoc::edit_anchor_points :
	{
		//NOTENOTE: save point for OnCommand handler
		PointF ptf = MousePointToMapPoint( point );
		m_EditorState.m_Point = ptf;
		m_GroupMenu.DestroyMenu();
		m_GroupMenu.CreatePopupMenu();
		FillAnchorMenu(m_GroupMenu);
		CPoint ptScr(point); 
		ClientToScreen(&ptScr);
		m_GroupMenu.TrackPopupMenu( 0, ptScr.x, ptScr.y, this );
		break;
	}

	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CVVMapEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if( !pDoc->IsCurrent() || !	pDoc->m_bMap || m_EditorState.m_State != EditorState::EDIT) return;

	switch(pDoc->m_State )
	{
		//TODO: Create map editing code
	case CVVMapEditorDoc::edit_map :	
	{
		break;
	}
	case CVVMapEditorDoc::edit_iz :
	{
		Zoom zoom(m_Scale, m_Scale); zoom.Offset = GetStartPoint() -  GetScrollPosition();
		PointF ptf = MousePointToMapPoint( point );
		m_EditorState.m_CurrentPoly->InvalidateEdit( this, zoom );
		m_EditorState.m_CurrentPoly->LButtonUp(ptf);
		m_EditorState.m_CurrentPoly->DrawEdit( this, zoom );
	//	Invalidate();
		break;
	}
	}
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void	CVVMapEditorView::DrawPoints(Graphics& gr/*, const CRect& rcClient*/)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent() &&
		pDoc->m_bShowCommonPoints)
	{
		SolidBrush br( Color::Red);
		Pen pen( Color::Black, 1);
		CPoint Start = GetStartPoint();
		const PointArr_t& pt = pDoc->Points().m_MapPoints;
		int nPointNumber = 0;

		std::unique_ptr<CFilterCache> FilterCache = pDoc->GetFilterCache();
		for( int i = 0;i < pt.size(); ++i )
		{
			if (pt[i].first && FilterCache->IsPointValid( i, c_nMapIndex ) )
			{
				PointF pts;
				pts.X = pt[i].second.X * m_Scale + Start.x;
				pts.Y = pt[i].second.Y * m_Scale + Start.y;
				DWORD dwColor = GetSettings().GetPointColor( i );
				br.SetColor( dwColor );
				gr.FillRectangle(&br, RectF( pts.X - 1, pts.Y - 1 ,  3,  3 ));
				gr.DrawRectangle(&pen,RectF( pts.X - 2, pts.Y - 2 ,  4,  4 ));
				++nPointNumber;
				if( pDoc->m_bShowPointNumber )	
					DrawHelper::DrawPointGroup( gr, pts, i, br);
			}
		}
		if( pDoc->m_bShowPointNumber )
		{
			CPoint Start = GetStartPoint();
			CSize sz = m_Background->GetSize();
			sz.cy = 0;
			PointF ptf( sz.cx * m_Scale + Start.x, sz.cy * m_Scale + Start.y);
			//PointF ptf = MousePointToMapPoint(pt);
			ptf.X -= 20;
			ptf.Y += 20;
			DrawPointNumber( gr, ptf, nPointNumber );
		}
	}
}

void	CVVMapEditorView::DrawPolygons(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	CPoint pt = GetStartPoint();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent() &&
		pDoc->m_bShowPolygons)
	{
		const PolygonArr_t& m = pDoc->m_MapPolygons;

		PolygonArr_t::const_iterator itr = m.begin();
		SolidBrush br( Color::Red );
		for(int Index = 0; itr != m.end() ; ++itr, ++Index)
		{
			const Polygon_t& p = *itr;
		
			int id = pDoc->IndexToId( Index );
			Color cl = GetSettings().GetMasterColor( id);
			cl.SetValue( Color::MakeARGB( 100, cl.GetR(), cl.GetG(), cl.GetB() ) );
			br.SetColor(cl);
			if(p.size())
			gr.FillPolygon( &br, &p[0], p.size() );
			
		}
	}
}

void	CVVMapEditorView::DrawIZ(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	CPoint pt = GetStartPoint();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent() &&
		pDoc->IsShowIZ()
		/*&&
		pDoc->m_bShowPolygons*/)
	{

		const InvZoneMap_t&	IZMap = pDoc->IZPolygons();
	
		Color clBack( Color::White );
		clBack.SetValue( Color::MakeARGB( 64, clBack.GetR(), clBack.GetG(), clBack.GetB() ) );
		for(int i=0; i < IZMap.size(); ++i)
		{
			if(! pDoc->IsIZView( IZMap[i].m_nMasterNumber) ) continue;
			const Polygon_t& arr = IZMap[i].m_Polygon;
			int id = pDoc->IndexToId( IZMap[i].m_nMasterNumber );
			Color cl = GetSettings().GetMasterColor( id);
			cl.SetValue( Color::MakeARGB( 0x80, cl.GetR(), cl.GetG(), cl.GetB() ) );
			HatchBrush br( HatchStyleLargeCheckerBoard, cl, clBack  );	
			if(arr.size()) gr.FillPolygon( &br, &arr[0], arr.size() );
			if( i == pDoc->GetActiveIZ() )
			{
				cl.SetValue( Color::MakeARGB( 0x80, cl.GetR(), cl.GetG(), cl.GetB() ) );
				Pen pen( Color::Black, 1);
				gr.DrawPolygon( &pen, &arr[0], arr.size() );
			}
		}
	}
}

void	CVVMapEditorView::DrawMapOrigin(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
//	CPoint pt = GetStartPoint();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent() &&
		pDoc->IsSetMapOrigin() )
	{
		Pen pen( Color::Red,1);
		SolidBrush br( Color::Red);
		PointF	ptf = pDoc->GetMapOrigin();
		PointF	Polygon [3] = 
		{
			PointF( ptf.X,			ptf.Y - 5 ),
			PointF( ptf.X + 7,		ptf.Y - 7 ),
			PointF( ptf.X,			ptf.Y - 10 ),
		};
		gr.FillPolygon( &br, Polygon, 3);
		gr.DrawLine( &pen, ptf.X, ptf.Y, ptf.X, ptf.Y - 10);
	}
}

void	DrawCross( Graphics& gr, long x, long y, Pen& pen)
{
	gr.DrawLine( &pen, PointF(x -3.0, y-3.0), PointF(x + 3.0, y+3.0) );
	gr.DrawLine( &pen, PointF(x -3.0, y +3.0), PointF(x + 3.0, y-3.0) );
}

void	CVVMapEditorView::DrawAnchorPoints(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	//NOTENOTE: old code
#if 0
	if( pDoc->IsCurrent()			&& 
		pDoc->IsMapInCurrent()		&&
		pDoc->GetScaleX().first		&&
		pDoc->GetScaleY().first		&&
		pDoc->IsSetMapOrigin()
		)
	{
		Pen pen(Color::Blue,2);
		const AnchorPointMap_t& ap = pDoc->AnchorPoints();
		double x,y,z;long lCalc;
		long err;
		for( int i=0; i < ap.size(); ++i)
		{
			err = pDoc->m_VVMapTrans->SP_GetAbsolute(i, &x, &y, &z, &lCalc);
			x = x * pDoc->GetScaleX().second + pDoc->GetMapOrigin().X;
			y = y * pDoc->GetScaleY().second + pDoc->GetMapOrigin().Y;
			Color cl = GetSettings().GetCustomColor(  cc_anchor );
			pen.SetColor( cl );
			gr.DrawLine( &pen, PointF(x -3.0, y-3.0), PointF(x + 3.0, y+3.0) );
			gr.DrawLine( &pen, PointF(x -3.0, y +3.0), PointF(x + 3.0, y-3.0) );
		}
	}
#endif
	if( pDoc->IsCurrent()			&& 
		pDoc->IsMapInCurrent()	
		)
	{
		Pen pen(Color::Blue,2);
		const AnchorPointMap_t& ap = pDoc->AnchorPoints();
		double x,y,z;long lCalc;
		long err;
//		long MapX, MapY;
		for( int i=0; i < ap.size(); ++i)
		{
			err = pDoc->m_VVMapTrans->SP_GetAbsolute(i, &x, &y, &z, &lCalc);
	//		lCalc < 0? pen.SetColor( Color::Green ) : pen.SetColor( Color::Blue );
/*
			err = pDoc->m_VVMapTrans->GetMapCoords( x, y, z, &MapX, &MapY );
			if( err == 0 )
			{
			//	ASSERT(FALSE);
				int z = 0;
			}
			else if( ap[i].IsOnMap() )
			{
				std::pair<long, long> p = ap[i].GetMapCoord();
				MapX = p.first;
				MapY = p.second;
			}
			else 
			{
				continue;
			}*/
			Color cl = GetSettings().GetProperty( ap[i].IsOnMap() ? prop_AnchorMapColor : prop_AnchorColor );
			pen.SetColor( cl );
			std::pair<bool, PointF> p = pDoc->GetAnchorPointOnMap( i );
			DrawCross( gr, p.second.X, p.second.Y , pen);
		}
	}
}

void	CVVMapEditorView::DrawRefPoints(Graphics& gr)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent()			&& 
		pDoc->IsMapInCurrent()		/*&&
		pDoc->GetScaleX().first		&&
		pDoc->GetScaleY().first		&&
		pDoc->IsSetMapOrigin()	*/	
		)
	{
		Color cl = GetSettings().GetProperty(  prop_RefPointColor );
		Pen pen(cl, 2);
		long err; long Number;
		long x,y;
		err = pDoc->m_VVMapTrans->RP_GetNumOfPoints(&Number);
		for( int i=0; i < Number; ++i)
		{
			err = pDoc->m_VVMapTrans->RP_GetMapCoords(i, &x, &y);
			//NOTENOTE: RP_GetMapCoords scale & offset auto
/*			x = x * pDoc->GetScaleX().second + pDoc->GetMapOrigin().X;
			y = y * pDoc->GetScaleY().second + pDoc->GetMapOrigin().Y;*/
			double c_nExtent = 5.0f;
			gr.DrawLine( &pen, PointF(x - c_nExtent, y-c_nExtent), PointF(x + c_nExtent, y+c_nExtent) );
			gr.DrawLine( &pen, PointF(x -c_nExtent, y +c_nExtent), PointF(x + c_nExtent, y-c_nExtent) );
		}
	}
}

///////////////////////////////****************** ///////////////////////////////////

CRect CVVMapEditorView::GetImageRect()
{
	if( m_Background.get())
	{
		CSize size = m_Background->GetSize();
		return CRect( GetStartPoint(), size );
	}
	else return CRect(0,0,0,0);
}

bool	CVVMapEditorView::CursorInRect(  )
{
		CRect rc;
		CPoint pt;
		GetCursorPos( &pt );
		GetClientRect( &rc );
		ScreenToClient(&pt);
		return rc.PtInRect( pt ) == TRUE;
}

void CVVMapEditorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	bool bInRect = CursorInRect();
	if( nIDEvent == IDT_GetPoint )
	{
		KillTimer( IDT_GetPoint );	
		if( !bInRect)
		{
			m_CrossDC.InvalidateCache(this);
		}
	}
	else if( nIDEvent == IDT_SelectPoint )
	{
		if( !bInRect )
		{
			KillTimer( IDT_SelectPoint );
			m_PointDC.InvalidateCache(this);
		}
		else
		{
			pDoc->DrawSelectPoint(m_nSelectGroup);
		}
	}
	else if( nIDEvent == IDT_AnchorPoint )
	{
		if( !bInRect)
		{
			KillTimer( IDT_AnchorPoint );
			m_AnchorDC.InvalidateCache(this);
		}
		else
		{
			pDoc->DrawAnchorPoint(m_nHilitedAnchorPoint);
		}
	}
	else if( nIDEvent == IDT_MapAbsCoords )
	{
		if( !bInRect)
		{
			KillTimer( IDT_AnchorPoint );
			InvalidateCoords();
		}
	}

	CScrollView::OnTimer(nIDEvent);
}

void CVVMapEditorView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
}

void CVVMapEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if( m_EditorState.m_State == EditorState::EDIT)
	{
		m_EditorState.m_State = EditorState::NONE;
		EndEdit();
	}	
	CScrollView::OnRButtonUp(nFlags, point);
}


void CVVMapEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CVVMapEditorDoc* pDoc = GetDocument();
	const int nRadius = 15;
	PointF ptf = MousePointToMapPoint( point );
	CRect rc( CPoint(ptf.X, ptf.Y), CSize(nRadius,nRadius) );
	rc.OffsetRect( -nRadius/2, -nRadius/2);
	if(	pDoc->IsCurrent() &&
		pDoc->IsMapInCurrent() &&
		pDoc->m_State == CVVMapEditorDoc::edit_points) 
	{	
		std::unique_ptr<CFilterCache> FilterCache = pDoc->GetFilterCache();
		const PointArr_t& v = pDoc->Points().m_MapPoints;
		for( int i=0; i < v.size(); ++i  )
		{
			if(  v[i].first && FilterCache->IsPointValid( i, c_nMapIndex ) )
			{			
				PointF ptf(v[i].second.X, v[i].second.Y);
		/*		ptf.X *= m_Scale;
				ptf.Y *= m_Scale;*/
				if( rc.PtInRect( CPoint(ptf.X, ptf.Y) ) )
				{
					m_EditorState.m_Number = i;
					m_EditorState.m_Point = PointF( point.x, point.y );
					m_EditorState.m_State = EditorState::EDIT;
					pDoc->Backup();
					StartEdit();
					break;
				}
			}
		}
		
	}
	else
	{
		CMenu* pMenu = GetMainWnd()->GetSubMenu(1);

		if( pDoc->IsCurrent() && 
			pDoc->IsMapInCurrent(  ) &&
			!pDoc->m_MapPolygons.empty()
			)
		{
			
		}	
		else
		{
			pMenu->EnableMenuItem( ID_MAP_EXPORT, MF_BYCOMMAND | MF_GRAYED );
		}

		CPoint pt(point);
		ClientToScreen(&pt);
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this );
	}

	CScrollView::OnRButtonDown(nFlags, point);
}


void CVVMapEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_wndToolTip.Set(point , _T(""));
	UpdateCoords(point);
	CVVMapEditorDoc* pDoc = GetDocument();
	if( m_EditorState.m_State == EditorState::EDIT &&
		pDoc->m_State == CVVMapEditorDoc::edit_points  )
	{
		PointF CurPoint(point.x, point.y);
		PointF & pt = pDoc->Points().m_MapPoints[m_EditorState.m_Number].second;
		pt.X += (CurPoint.X - m_EditorState.m_Point.X)/m_Scale ;
		pt.Y += (CurPoint.Y - m_EditorState.m_Point.Y)/m_Scale ;
		m_EditorState.m_Point = CurPoint;
		Invalidate();
	}	

	if( !pDoc->IsCurrent()	||	!pDoc->IsMapInCurrent() ) return;

	if( m_EditorState.m_State == EditorState::EDIT && 
		pDoc->m_State == CVVMapEditorDoc::edit_iz )
	{
		Zoom zoom(m_Scale, m_Scale); 
		PointF ptf = MousePointToMapPoint(point) ;//(point.x/ m_Scale, point.y/m_Scale) ;
		zoom.Offset = GetStartPoint()  - GetScrollPosition();
		if( m_EditorState.m_CurrentPoly->NeedRedrawWhenMousMove() )
			m_EditorState.m_CurrentPoly->InvalidateEdit(this, zoom);
		m_EditorState.m_CurrentPoly->MouseMove(ptf);
		if( m_EditorState.m_CurrentPoly->NeedRedrawWhenMousMove() )
			m_EditorState.m_CurrentPoly->DrawEdit(this, zoom);
	}
//NOTENOTE: new code 09-17-2004
	if( pDoc->m_State == CVVMapEditorDoc::edit_anchor_points/*	&&
		pDoc->GetScaleX().first									&& 
		pDoc->GetScaleY().second								&&
		pDoc->IsSetMapOrigin()									*/	)
	{
		bool bHilitePoint = false;
	/*	
		PointF ptf = MousePointToMapPoint( point );
		const AnchorPointMap_t& apm = pDoc->AnchorPoints();
		for(int i=0; i < apm.size(); ++i)
		{
			PointF ptf = pDoc->GetAnchorPointOnMap(i);
			CPoint ptStart = MapPointToMousePoint( ptf );
			CRect rc( ptStart, CSize(10,10) );
			rc.OffsetRect(-5,-5);
			if(rc.PtInRect( point ))
			{
				m_AnchorDC.InvalidateCache(this);
				m_nHilitedAnchorPoint = i;
				SetTimer( IDT_AnchorPoint, 300, 0 );
				bHilitePoint = true;
				break;
			}
		}*/
		int nAnchor = FindAnchorPointOnMap(point);
		if( nAnchor != -1 )
		{
			m_AnchorDC.InvalidateCache(this);
			m_nHilitedAnchorPoint = nAnchor;
			SetTimer( IDT_AnchorPoint, 300, 0 );
			bHilitePoint = true;
		}
/*		if( !bHilitePoint )*/
		else
		{
			KillTimer( IDT_AnchorPoint );
			pDoc->InvalidateAnchorPoint(  );
		}
	}
//NOTENOTE: end of new 
	if(	pDoc->m_bShowGetPoints)
	{
		PointF CurPoint(point.x, point.y);
		CPoint Start = GetStartPoint();
		CSize sizeScroll = GetScrollPosition();
		CPoint pt = point - Start + sizeScroll;
		pt.x /= m_Scale;
		pt.y /= m_Scale;

		int Index = pDoc->m_Ids.size();
		pDoc->m_GetPoint = std::make_pair( Index, pt );
		m_EditorState.m_Point = CurPoint;
		pDoc->DrawGetPoint();
		SetTimer( IDT_GetPoint, 100, 0 );
	}

	std::unique_ptr<CFilterCache> FilterCache = pDoc->GetFilterCache();
		const PointArr_t& Arr = pDoc->Points().m_MapPoints;
		bool set = false;
		for( int i=0;  i < Arr.size(); ++i)
		{
			if( Arr[i].first && FilterCache->IsPointValid( i, c_nMapIndex ) )
			{
				m_PointDC.InvalidateCache(this);
				CPoint ptStart = MapPointToMousePoint( Arr[i].second );
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

	CScrollView::OnMouseMove(nFlags, point);
}

void	CVVMapEditorView::StartEdit()
{
	SetCapture();
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	ClipCursor(&rc);
}

void	CVVMapEditorView::EndEdit()
{
	ClipCursor(0);
	ReleaseCapture();
	m_EditorState.m_State = EditorState::NONE;
}

void CVVMapEditorView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	// TODO: Add your message handler code here
	if(m_EditorState.m_State == EditorState::NONE) return;
	
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->m_State == CVVMapEditorDoc::edit_iz &&
		m_EditorState.m_State == EditorState::EDIT)
	{
		CancelEditIZ();
	}
	else
		EndEdit();
	Invalidate();
	
}

BOOL CVVMapEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if(	pDoc->IsCurrent() &&
		pDoc->m_State == CVVMapEditorDoc::edit_points &&
		pDoc->IsMapInCurrent())
	{
		SetCursor( m_hCross );
		return TRUE;
	}
	
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

void	CVVMapEditorView::DrawGetPoints( CDC* pDC )
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent(  ) &&
		pDoc->m_bShowGetPoints)
	{
		long x = pDoc->m_GetPoint.second.x;
		long y = pDoc->m_GetPoint.second.y;
		long xto, yto;

		int Index = pDoc->m_Ids.size();
		if( pDoc->m_GetPoint.first == Index ) return;
		long err = pDoc->m_VVMapTrans->GetXY( pDoc->m_GetPoint.first, x, y, Index, &xto, &yto  );

		CPoint Start = GetStartPoint();
		xto = xto * m_Scale + Start.x;
		yto = yto * m_Scale + Start.y;
//		ASSERT(err == 30);

		CSize sizeCross (	GetSettings().GetProperty( prop_CrossExtent ),
							GetSettings().GetProperty( prop_CrossThickness ) );
		m_CrossDC.Draw( pDC, CCrossCache( CPoint(xto, yto), err == ERR_NO_ERROR/*0*/, sizeCross.cx/2, sizeCross.cy ) );
	}
}


void	CVVMapEditorView::Scale(float scale)
{
	m_Scale = scale;
	if( m_Background.get() )
	{
/*		CVVMapEditorDoc* pDoc = GetDocument();
		if( pDoc->IsCurrent() && 
			pDoc->IsMapInCurrent(  ) &&
			pDoc->HasMapPoints())
		{

		}*/
//		pDoc->ScaleMap(scale);
		CSize size = m_Background->GetSize();
		size.cx *= m_Scale;
		size.cy *= m_Scale;
		SetScrollSizes( MM_TEXT, size );
		Invalidate();
	}

}
void CVVMapEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	GetMainWnd()->GetPointTree()->Refresh();
	Invalidate();
}

void CVVMapEditorView::OnMapExport() 
{
	// TODO: Add your command handler code here
	CString Filter;
	Filter.LoadString( IDS_FILTER_POLYGON );
	CFileDialogEx dlg ( FALSE, _T(".mpf") , NULL,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, Filter, this);
	if(dlg.DoModal() != IDOK) return;	
	
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetDocument();
	pDoc->ExportMapPolygon( dlg.GetPathName() );	
}

void CVVMapEditorView::OnUpdateMapExport(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CVVMapEditorDoc* pDoc = (CVVMapEditorDoc*)GetDocument();

	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent(  ) &&
		!pDoc->m_MapPolygons.empty()
		)
	{
		pCmdUI->Enable( TRUE );
	}	
	else
	{
		pCmdUI->Enable( FALSE );
	}		
}

void CVVMapEditorView::OnDynamicGroupMenu( UINT nID )
{
	int nGroup = nID - ID_MENU_GROUP;


	CVVMapEditorDoc* pDoc = GetDocument();
	bool bNew = (nGroup == 0);
	
	--nGroup;

	PointF ptf = m_EditorState.m_Point;
	pDoc->AddPointToGroup( bNew? -1 : nGroup, -1, ptf );
	GetMainWnd()->GetPointTree()->Refresh();		
}

void	CVVMapEditorView::DrawGetPoint()
{
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
			m_StateCache.DrawInvert( this  );
			return;
		}
		long err = pDoc->m_VVMapTrans->GetXY( pDoc->m_GetPoint.first, x, y, Index, &xto, &yto  );

		CPoint Start = GetStartPoint();
//		Start.Offset( -GetScrollPosition() );
		xto = xto * m_Scale + Start.x;
		yto = yto * m_Scale + Start.y;

		CSize sizeCross (	GetSettings().GetProperty( prop_CrossExtent ),
							GetSettings().GetProperty( prop_CrossThickness ) );

		m_StateCache.DrawInvert( this  );
		m_StateCache = CStateCache( CPoint(5,5), err, this );
		m_StateCache.Draw( this  );

		CCrossCache cache( CPoint(xto, yto), err == ERR_NO_ERROR/*0*/, sizeCross.cx/2, sizeCross.cy  );
		m_CrossDC.Draw( this, cache );
	}	
}

void	CVVMapEditorView::DrawSelectPoint(int nGroupNumber)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent( ))
	{
		const std::pair<bool, PointF>& ptz = pDoc->Points().m_MapPoints[nGroupNumber];

		if(ptz.first)
		{
			CPoint pt  = MapPointToMousePoint(ptz.second);
			pt.Offset( 2, 2 );
			CPointCache cache( pt + GetScrollPosition() );
			m_PointDC.Draw( this, cache );
		}
	}	
}

void	CVVMapEditorView::DrawAnchorPoint(int nPoint)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent( ))
	{
//		const std::pair<bool, PointF>& ptz = pDoc->RefPoints();
		std::pair<bool, PointF> ptf = pDoc->GetAnchorPointOnMap( nPoint );
		if( ptf.first )
		{
			CPoint pt  = MapPointToMousePoint(ptf.second);
			CAnchorCache cache( pt + GetScrollPosition(), 5 );
			m_AnchorDC.Draw( this, cache );
		}
	}
}


void	CVVMapEditorView::DrawRefPoint(int nPoint)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsMapInCurrent( ))
	{

	}
}

void CVVMapEditorView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CVVMapEditorDoc* pDoc = GetDocument();

	if( m_EditorState.m_State == EditorState::EDIT && 
		pDoc->m_State == CVVMapEditorDoc::edit_iz )
	{
		PointF ptf = MousePointToMapPoint( point );
		m_EditorState.m_CurrentPoly->LButtonDbl( ptf );
			const std::vector<PointF>& ptVec = m_EditorState.m_CurrentPoly->GetPoints();
		//NOTENOTE: Add when polyline has more than 2 points, neither dot nor line
		if(  ptVec.size() > 2 )
		{
			int nActiveZone = pDoc->AddIZPolygon( ptVec );
			pDoc->SetActiveIZ( nActiveZone );
		}
		m_EditorState.m_CurrentPoly = std::unique_ptr<CFigPolyline>();
		m_EditorState.m_State = EditorState::NONE;
		Invalidate();
		EndEditIZ();
	}
		
	CScrollView::OnLButtonDblClk(nFlags, point);
}

//-----------------------------------------------------------------------------
// Purpose: when start edit IZ we do some initialization clipping e.g.
//-----------------------------------------------------------------------------
void	CVVMapEditorView::StartEditIZ()
{
	SetCapture();
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	ClipCursor(&rc);
	m_EditorState.m_State = EditorState::EDIT;
}

//-----------------------------------------------------------------------------
// Purpose: when edit IZ is almost finished
//-----------------------------------------------------------------------------
void	CVVMapEditorView::EndEditIZ()
{
	ClipCursor(0);
	ReleaseCapture();
	m_EditorState.m_CurrentPoly = std::unique_ptr<CFigPolyline>();
	m_EditorState.m_State = EditorState::NONE;
}

//-----------------------------------------------------------------------------
// Purpose: user cancel edit or focus was lose, handle it
//-----------------------------------------------------------------------------
void	CVVMapEditorView::CancelEditIZ()
{
	EndEditIZ();
}


//-----------------------------------------------------------------------------
// Purpose: conversion for device coodinates to virtual map 
// Input  : point - usually from mouse handler
//-----------------------------------------------------------------------------
PointF	CVVMapEditorView::MousePointToMapPoint(CPoint point) const
{
	CPoint Start = GetStartPoint();
	CPoint pt = GetScrollPosition();
	SizeF size (pt.x + point.x - Start.x, pt.y + point.y - Start.y);
	size.Width /= m_Scale;
	size.Height /= m_Scale;
	return PointF(size.Width, size.Height);
}

//-----------------------------------------------------------------------------
// Purpose: conversion from map point to device, usually for mouse handler
// Input  : point - map point
//-----------------------------------------------------------------------------
CPoint	CVVMapEditorView::MapPointToMousePoint(PointF point) const
{
	long xto, yto;
	CPoint Start = GetStartPoint();
	xto = point.X * m_Scale + Start.x - GetScrollPos(SB_HORZ);
	yto = point.Y * m_Scale + Start.y - GetScrollPos(SB_VERT);
	return CPoint( xto, yto );
}

void CVVMapEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CVVMapEditorDoc* pDoc = GetDocument();
	if(nChar == VK_ESCAPE)
		Escape();
	if(nChar == VK_DELETE && pDoc->GetActiveIZ() != -1)
	{
		pDoc->RemoveIZPolygon( pDoc->GetActiveIZ() );
		pDoc->SetActiveIZ();
		Invalidate();
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void	CVVMapEditorView::Escape()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( m_EditorState.m_State == EditorState::EDIT && 
		pDoc->m_State == CVVMapEditorDoc::edit_iz )
	{
		CancelEditIZ();
		Invalidate();
	}
}

void	CVVMapEditorView::DrawPointGroup( Graphics& gr, PointF point, int Number, Brush& br)
{
	DrawHelper::DrawPointGroup(gr, point, Number, br);
}

void	CVVMapEditorView::DrawPointNumber( Graphics& gr, PointF point, int Number)
{
	SolidBrush br(GetSettings().GetProperty( prop_PointNumberColor )  );
	DrawHelper::DrawPointNumber( gr, point , Number, br );
}


void	CVVMapEditorView::FillAnchorMenu( CMenu& menu )
{
	CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& ap = pDoc->AnchorPoints();

	CString strText;
	for( int i=0; i < ap.size(); ++i)
	{
		if( !ap[i].IsOnMap() )
		{
			strText.Format( _T("Point %d"), i );
			menu.AppendMenu( MF_BYPOSITION, ID_MENU_ANCHOR + i, strText );
		}
	}
}

void	CVVMapEditorView::OnDynamicAnchorMenu( UINT nID )
{
	int nIndex = nID - ID_MENU_ANCHOR;
	ASSERT( nIndex >= 0 && nIndex < 100 );

	CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& ap = pDoc->AnchorPoints();

	for( int i=0, ind = 0; i < ap.size(); ++i)
	{
		if( !ap[i].IsOnMap() && nIndex == ind)
		{
			CPoint pt ( m_EditorState.m_Point.X, m_EditorState.m_Point.Y );
			pDoc->SetAnchorPointOnMap(i , pt.x, pt.y  );
			Invalidate();
			break;
		}
		else 
			++ind;
	}
}

int		CVVMapEditorView::FindAnchorPointOnMap(CPoint point)
{
	PointF ptf = MousePointToMapPoint( point );
	CVVMapEditorDoc* pDoc = GetDocument();
	const AnchorPointMap_t& apm = pDoc->AnchorPoints();
	for(int i=0; i < apm.size(); ++i)
	{
		std::pair<bool, PointF> ptf = pDoc->GetAnchorPointOnMap(i);
		if( ptf.first )
		{
			CPoint ptStart = MapPointToMousePoint( ptf.second );
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

BOOL CVVMapEditorView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_wndToolTip.PreTranslateMessage(pMsg);
	return CScrollView::PreTranslateMessage(pMsg);
}

int		CVVMapEditorView::FindCorrPointOnMap( CPoint point )
{
	CVVMapEditorDoc* pDoc = GetDocument();

	//NOTENOTE: 10/15/2004 now with point filter
	if( !pDoc->m_bShowCommonPoints ) return -1;
	const PointArr_t& Arr = pDoc->Points().m_MapPoints;
	std::unique_ptr<CFilterCache> FilterCache = pDoc->GetFilterCache();
	for( int i=0;  i < Arr.size(); ++i)
	{
		if( Arr[i].first && FilterCache->IsPointValid( i, c_nMapIndex ) )
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

void CVVMapEditorView::OnNotifyTooltip(NMHDR * pNMHDR, LRESULT * result)
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

void	CVVMapEditorView::UpdateCoords( CPoint ptMouse )
{
	CWnd * pWnd = AfxGetMainWnd();
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(pWnd);
	if( pFrame )
	{
		CVVMapEditorDoc* pDoc = GetDocument();
		
		SetTimer( IDT_MapAbsCoords, 100, 0 );
		CString strText = _T("");
		if (  GetBackground() )
		{
			PointF ptf = MousePointToMapPoint( ptMouse ) ;
			long x = ptf.X, y = ptf.Y;
		
			strText.Format( _T("%d,%d"), x, y);
			pFrame->SetMapCoords( strText );

			double fX, fY, fZ; 
			long err = pDoc->m_VVMapTrans->GetCoordsFromMap( &fX, &fY, &fZ, x , y);
			if( err == ERR_NO_ERROR )
			{
				strText.Format( _T("%.2f,%.2f,%.2f"), fX, fY, fZ);
				pFrame->SetAbsCoords( strText );
			}
			else
				pFrame->SetAbsCoords( _T("") );
		}
		else
			pFrame->SetMapCoords( _T("") );
	}
}

void	CVVMapEditorView::InvalidateCoords()
{
	CWnd * pWnd = AfxGetMainWnd();
	CMainFrame* pFrame = dynamic_cast<CMainFrame*>(pWnd);
	if( pFrame )
	{
		CString strText = _T("");
		pFrame->SetMapCoords( strText );
		pFrame->SetAbsCoords( _T("") );
	}	
}

void CVVMapEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_StateCache.DrawInvert(this);

	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVVMapEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_StateCache.DrawInvert(this);
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CVVMapEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	m_StateCache.DrawInvert(this);
	
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
/*
bool	CVVMapEditorView::SetMapInfo()
{

}
*/
bool	CVVMapEditorView::GetMapInfo()
{
	CVVMapEditorDoc* pDoc = GetDocument();

	BITMAPINFO* pbi;
	BYTE*		pbits;	
	long res = pDoc->m_VVMapTrans->GetMapBitmap( &pbi, &pbits );
	if(!res && pbits && pbi)
	{
		BITMAPINFO bi;
		memcpy(&bi, pbi, sizeof(BITMAPINFO));
		bi.bmiHeader.biClrImportant = 0;
		bi.bmiHeader.biClrUsed = 0;
		bi.bmiHeader.biCompression = BI_RGB;
		bi.bmiHeader.biPlanes = 1;
		try
		{
			m_Background = std::unique_ptr<CBackground>(new CBitmapBackground(&bi, pbits) );
		}
		catch( ImageFileException& )
		{
			return false;
		}
		CSize size = m_Background->GetSize();
		SetScrollSizes( MM_TEXT, size );
		return true;
	} 

	const void* pData; size_t nSize;
	res = pDoc->m_VVMapTrans->GetMapBuffer( &pData, &nSize );
	if( res == ERR_OK && nSize && pData )
	{
		try
		{
		m_Background = std::unique_ptr<CBackground>(new CImageBackground( pData, nSize) );
		}
		catch( CommonException )
		{
			AfxMessageBox( IDS_EXC_INVALID_IMAGE_FILE, MB_OK|MB_ICONERROR );
			m_Background = std::unique_ptr<CBackground>(new CEmptyBackground( ) );
		}
	}
	return false;
}