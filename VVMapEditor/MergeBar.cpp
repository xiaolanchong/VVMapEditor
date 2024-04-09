// MergeBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "VVMapEditorDoc.h"
#include "MergeBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeBar

IMPLEMENT_DYNAMIC( CMergeBar, CGuiControlBar );

CMergeBar::CMergeBar( CWnd * pParentWnd, CString sTitle,CRect rcClient, int Index):
	m_rcClient( rcClient )
{
	CGuiControlBar::Create( sTitle, WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, Index + 0x100 );

	CRect rect;
	GetClientRect(&rect);

	CCreateContext cc;
	memset( &cc, 0, sizeof(cc));

	m_ctMerge.Create(WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,124);

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CDocument* pDoc = pTmpl->GetNextDoc(pos);
	m_pView = (CMergeView*)RUNTIME_CLASS(CMergeView)->CreateObject();
	m_pView->m_Index = Index;
	m_pView->Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,CRect(0,0,0,0), &m_ctMerge,0, &cc);
	pDoc->AddView( m_pView );
	m_pView->OnInitialUpdate();

	//SetWindowPos( 0, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER|SWP_NOMOVE);
	NCCALCSIZE_PARAMS nccp;
	memset( &nccp, 0, sizeof (NCCALCSIZE_PARAMS) );
	//NOTENOTE: OnNcCalcSize tribute this format since it subtrate form bounds
	nccp.rgrc[0].top	= - rcClient.top ;
	nccp.rgrc[0].left	= - rcClient.left ;
	nccp.rgrc[0].bottom = - rcClient.bottom ;
	nccp.rgrc[0].right	= - rcClient.right ;
	OnNcCalcSize( FALSE, &nccp );
	nccp.rgrc[0].top	= - nccp.rgrc[0].top ;
	nccp.rgrc[0].left	= - nccp.rgrc[0].left ;
	nccp.rgrc[0].bottom = - nccp.rgrc[0].bottom ;
	nccp.rgrc[0].right	= - nccp.rgrc[0].right ;
	CRect rcWnd ( nccp.rgrc[0] );
	m_sizeMinFloating = rcWnd.Size();

	m_dwAllowDockingState = CBRS_ALIGN_ANY;

}

CMergeBar::~CMergeBar()
{
}

void	CMergeBar::OnCloseBtn()
{
	CWnd*	pFrame = (CWnd*)AfxGetMainWnd();
	CVVMapEditorDoc * pDoc = (CVVMapEditorDoc*)m_pView->GetDocument();	
	pDoc->RemoveView(m_pView);
	int z = m_pView->m_Index;
	m_pView->DestroyWindow();
	OutputDebugString(_T("CMergeBar::OnCloseBtn\n"));
	pFrame->PostMessage( WM_CLOSE_MERGE, z );
//	delete m_pView;
}


BEGIN_MESSAGE_MAP(CMergeBar, CGuiControlBar)
	//{{AFX_MSG_MAP(CMergeBar)
	ON_WM_NCRBUTTONDOWN()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_CB_ALLOW_DOCKING, OnAllowDocking)
	ON_COMMAND(ID_CB_CLOSE, OnCloseB)	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeBar message handlers

CSize CMergeBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

//	CGuiControlBar::CalcFixedLayout( bStretch, bHorz );
	return CalcLayout(dwMode);
}

CSize CMergeBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);


	CSize sizeResult(0,0);

	//BLOCK: Load Buttons

	sizeResult += m_pView->m_rcArea.Size();

	if( IsFloating() )
	{
		int z = 0;
	}
	else 
	{
		int z = 0;
	}
	{
		CRect rc;
//FIXME:  Check CalcDynamicLayout&CalcFixedLayout troubles"
		CControlBar::CalcInsideRect( rc, dwMode & LM_HORZ );
		sizeResult.cx += rc.Width() + 28 ;
		sizeResult.cy += rc.Height() + 35;
	}

	return sizeResult;
}

CSize CMergeBar::CalcDynamicLayout(int nLength, DWORD nMode)
{	
	if (IsFloating())
	{
		// Enable diagonal arrow cursor for resizing
		//m_sizeVert=m_sizeHorz=CSize(200,200); 
		GetParent()->GetParent()->ModifyStyle(MFS_4THICKFRAME|WS_CAPTION,0);
	} 
	if (nMode & (LM_HORZDOCK | LM_VERTDOCK)) 
	{
		m_pDockSite->DelayRecalcLayout();
		//obligar a reposicionar  la ventana, de lo contrario cuando vuelva de un doble click
		//desde la ventana CMiniFrameWnd queda sin area cliente
		SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_FRAMECHANGED|SWP_NOREDRAW);
	 	return CControlBar::CalcDynamicLayout(nLength, nMode);
	}
	if (nMode & LM_MRUWIDTH)
        return  m_sizeMinFloating;

    if (nMode & LM_COMMIT)
        return  m_sizeMinFloating ;
    
	if (IsFloating())
	{
		CSize sizeMaxArea = CalcLayout(LM_HORZ, 1);

		CRect	rcWin;
		POINT	cpt;
		GetCursorPos(&cpt);
		GetParentFrame()->GetWindowRect(&rcWin);
		int nXOffset=0;int nYOffset=0;
		switch (m_pDockContext->m_nHitTest)
		{
# undef max 
			//------------------------------------------------------------------
			case HTLEFT:
				m_pDockContext->m_rectFrameDragHorz= rcWin;
				m_pDockContext->m_rectFrameDragHorz.left = cpt.x;
				m_sizeMinFloating.cx = std::max<int>(rcWin.right - cpt.x,32)-4 ;
				m_sizeMinFloating.cy = std::max<int>((rcWin.bottom -rcWin.top)-nGapGripper-5,32)+2 ;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );

				return m_sizeMinFloating;		
			break;
			case HTTOP:
				m_pDockContext->m_rectFrameDragHorz=rcWin;
				m_pDockContext->m_rectFrameDragHorz.top = cpt.y;
				m_sizeMinFloating.cx = std::max<int>(rcWin.right-rcWin.left-2,32)-4 ;
				m_sizeMinFloating.cy = std::max<int>((rcWin.bottom -nGapGripper-cpt.y-3),32) ;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );
				return m_sizeMinFloating;		
			break;
			case HTRIGHT:
				m_pDockContext->m_rectFrameDragHorz=rcWin;
				m_pDockContext->m_rectFrameDragHorz.right = cpt.x;
				m_sizeMinFloating.cy = std::max<int>(rcWin.bottom -rcWin.top-nGapGripper-3,32) ;
				m_sizeMinFloating.cx = std::max<int>(cpt.x-rcWin.left-4,32);

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );
				
				return m_sizeMinFloating;	
			break;
			case HTBOTTOM:
				m_pDockContext->m_rectFrameDragHorz=rcWin;
				m_sizeMinFloating.cy = std::max<int>(cpt.y-rcWin.top -nGapGripper-3,32) ;
				m_sizeMinFloating.cx = std::max<int>(rcWin.right-rcWin.left-2,32)-4 ;
				m_pDockContext->m_rectFrameDragHorz.bottom = cpt.y-4;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );

				return m_sizeMinFloating;
				break;
			case HTTOPLEFT:
				//---------------------------------------------------------
			//En este caso crece la ventana a izquierda y hacia arriba
			//izquierda incrementa cx y top incrementa cy
				m_sizeMinFloating.cx = std::max<int>(rcWin.right - cpt.x,32)-3 ;
				m_sizeMinFloating.cy = std::max<int>(rcWin.bottom -nGapGripper-cpt.y,32)-2 ;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );

				m_pDockContext->m_rectFrameDragHorz.top = cpt.y-1;
				m_pDockContext->m_rectFrameDragHorz.left = cpt.x-2;

				return m_sizeMinFloating;		
				break;
			case HTTOPRIGHT:
				m_sizeMinFloating.cx = std::max<int>(cpt.x-rcWin.left,32)-4 ;
				m_sizeMinFloating.cy = std::max<int>(rcWin.bottom -nGapGripper-cpt.y,32)-2 ;
				m_pDockContext->m_rectFrameDragHorz.top = cpt.y-1;
				m_pDockContext->m_rectFrameDragHorz.right = cpt.x-2;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );

				return m_sizeMinFloating;		
				break;
			case HTBOTTOMLEFT:
				m_sizeMinFloating.cx = std::max<int>(rcWin.right - cpt.x,32)-4;
				m_sizeMinFloating.cy = std::max<int>(cpt.y-rcWin.top -nGapGripper,32)-2 ;
				m_pDockContext->m_rectFrameDragHorz.top = rcWin.top;
				m_pDockContext->m_rectFrameDragHorz.bottom = cpt.y-1;
				m_pDockContext->m_rectFrameDragHorz.left = cpt.x-2;
				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );
				
				return m_sizeMinFloating;		
				break;
			case HTBOTTOMRIGHT:
				m_sizeMinFloating.cx = std::max<int>(cpt.x-rcWin.left,32);
				m_sizeMinFloating.cy = std::max<int>(cpt.y-rcWin.top -nGapGripper,32) ;
				m_pDockContext->m_rectFrameDragHorz.top = rcWin.top;
				m_pDockContext->m_rectFrameDragHorz.bottom = cpt.y+1;
				m_pDockContext->m_rectFrameDragHorz.right = cpt.x+2;

				m_sizeMinFloating.cx = min( m_sizeMinFloating.cx, sizeMaxArea.cx );
				m_sizeMinFloating.cy = min( m_sizeMinFloating.cy, sizeMaxArea.cy );
				
				return m_sizeMinFloating;		
				break;
		}
		
	}

	if(nMode & LM_LENGTHY)
		 m_sizeMinFloating.cy = std::max<int>(nLength,32);
	else
	     m_sizeMinFloating.cx = std::max<int>(nLength,32);
	return m_sizeMinFloating;

}
void CMergeBar::OnNcRButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( nHitTest == HTCAPTION )
	{

	m_SysMenu.DestroyMenu();
	m_SysMenu.CreatePopupMenu();
	m_MenuManager.LoadToolbar(IDR_TOOLBAR1);

	
	Invalidate();
	UpdateWindow();

	DWORD dwDockStyle = m_dwDockStyle;
	CString strMenu;
	if ((dwDockStyle != 0 || m_dwAllowDockingState != 0))	// ie docking is actually allowed ...
	{
		DWORD style = (dwDockStyle != 0 ? MF_STRING | MF_CHECKED : MF_STRING);
		VERIFY(strMenu.LoadString(ID_CB_ALLOW_DOCKING));
		m_SysMenu.AppendMenu(style, ID_CB_ALLOW_DOCKING, strMenu);
	}
	VERIFY(strMenu.LoadString(ID_CB_CLOSE));
	m_SysMenu.AppendMenu(MF_STRING, ID_CB_CLOSE, strMenu);

	m_SysMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			point.x, point.y-2, /*AfxGetMainWnd()*/this);	
	}


//	CGuiControlBar::OnNcRButtonDown(nHitTest, point);
}

void CMergeBar::OnAllowDocking()
{
	CGuiControlBar::OnAllowDocking();	
}

void CMergeBar::OnCloseB()
{
	OnCloseBtn();	
}

/////////////////////////////////////////////

BOOL	CMapPanoramaBar::Create( CWnd * pParentWnd, CString sTitle)
{
	CGuiControlBar::Create( sTitle, WS_CLIPCHILDREN|WS_CHILD | /*WS_VISIBLE |*/ 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, 0x300 );



	CCreateContext cc;
	memset( &cc, 0, sizeof(cc));

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CDocument* pDoc = pTmpl->GetNextDoc(pos);
	m_pView = (CMapPanoramaView*)RUNTIME_CLASS(CMapPanoramaView)->CreateObject();
//	m_pView = ::new CMapPanoramaView();
//	m_pView->m_Index = -1;
	m_pView->Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,CRect(0,0,0,0), this,0, &cc);
	pDoc->AddView( m_pView );
	m_pView->OnInitialUpdate();

	EnableDocking(0);
	//m_pView->SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE|SWP_NOZORDER);
	GetParentFrame()->FloatControlBar( this, CPoint(0,0) );
	GetParentFrame()->RecalcLayout();

//	m_dwAllowDockingState = CBRS_ALIGN_ANY;
	return TRUE;
}

void	CMapPanoramaBar::CreateBitmap()
{
	CSize sizeClient = m_pView->CreateBitmap();


	CRect rcClient ( CPoint(0,0), sizeClient );
	//SetWindowPos( 0, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER|SWP_NOMOVE);
	NCCALCSIZE_PARAMS nccp;
	memset( &nccp, 0, sizeof (NCCALCSIZE_PARAMS) );
	//NOTENOTE: OnNcCalcSize tribute this format since it subtrate form bounds
	nccp.rgrc[0].top	= - rcClient.top ;
	nccp.rgrc[0].left	= - rcClient.left ;
	nccp.rgrc[0].bottom = - rcClient.bottom ;
	nccp.rgrc[0].right	= - rcClient.right ;
	OnNcCalcSize( FALSE, &nccp );
	nccp.rgrc[0].top	= - nccp.rgrc[0].top ;
	nccp.rgrc[0].left	= - nccp.rgrc[0].left ;
	nccp.rgrc[0].bottom = - nccp.rgrc[0].bottom ;
	nccp.rgrc[0].right	= - nccp.rgrc[0].right ;
	CRect rcWnd ( nccp.rgrc[0] );
	m_sizeMinFloating = rcWnd.Size();
	

	GetParentFrame()->RecalcLayout();
}

IMPLEMENT_DYNCREATE(CMapPanoramaView, CScrollView)

CMapPanoramaView::CMapPanoramaView():m_Bmp(0)
{
}

CMapPanoramaView::~CMapPanoramaView()
{
	if(m_Bmp) ::delete m_Bmp;
}


BEGIN_MESSAGE_MAP(CMapPanoramaView, CScrollView)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeView drawing

void CMapPanoramaView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


CSize	CMapPanoramaView::CreateBitmap()
{
	CVVMapEditorDoc* pDoc = static_cast<CVVMapEditorDoc*>(GetDocument());
	Bitmap *Bmp = pDoc->CreateMapBitmap( );
	SetBitmap( Bmp );
	return Bmp? CSize( Bmp->GetWidth(), Bmp->GetHeight() ) : CSize(0,0);
}

void CMapPanoramaView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	Graphics gr(pDC->GetSafeHdc());

	if(m_Bmp) gr.DrawImage( m_Bmp, 0, 0 );
}

void	CMapPanoramaView::SetBitmap(Bitmap* bmp) 
{ 	
	if( m_Bmp)
	{
		::delete m_Bmp; 
	}
	m_Bmp = bmp;
	if( m_Bmp )
	{
		SetWindowPos( 0, 0, 0, m_Bmp->GetWidth(), m_Bmp->GetHeight(), SWP_NOZORDER|SWP_NOMOVE );
	}	
}