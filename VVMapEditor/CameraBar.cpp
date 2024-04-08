// CameraBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "CameraBar.h"
#include "VideoBackground.h"
#include "RealtimeBackground.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraBar

const int nID_Start = 0x100;

const int IDT_PauseButton = 1000;

CCameraBar::CCameraBar(int nID, CWnd* pParentWnd ) : m_statePause(NORMAL)
{
	CString strCaption;
//	strCaption.Format( _T("ID = %d"), nID);
	strCaption.Format( IDS_FMT_ID , nID);
	CGuiControlBar::Create( strCaption, WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, nID + nID_Start );

	CCreateContext cc;
	memset( &cc, 0, sizeof(cc));
	CFrameWnd*	pFrame = (CFrameWnd*)/*AfxGetMainWnd()*/pParentWnd;

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CDocument* pDoc = pTmpl->GetNextDoc(pos);
	cc.m_pCurrentDoc = pDoc;
	cc.m_pCurrentFrame = pFrame;
	cc.m_pLastView = pFrame->GetActiveView();
	cc.m_pNewViewClass = RUNTIME_CLASS(CCameraView);	

	LoadStateBar( CString(_T("CameraBars") ));

//	m_ctCamera.Create(WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,124);
	CString strPause;
	strPause.LoadString( IDS_PAUSE );
	m_PauseButton.SetData(0, strPause);
	m_PauseButton.SetImageList( IDB_PAUSE, 9, 1, RGB(255,0,0) );

	m_pView = (CCameraView*)RUNTIME_CLASS(CCameraView)->CreateObject();
	m_pView->m_Number = nID;
	m_pView->Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,CRect(0,0,352,288),this/*&m_ctCamera*/, 1, &cc);

	m_pView->SendMessage( WM_INITIALUPDATE);

	SetKeepSizeWhenDocked();
//	m_sizeVert = m_sizeHorz = CSize(358, 302);
//	pDoc->AddView( m_pView );
}

CCameraBar::~CCameraBar()
{
}

void	CCameraBar::SetNumber( int nID )
{
//	CString Name;
//	Name.Format( _T("ID = %d"), nID );
	CString strCaption;
	strCaption.Format( IDS_FMT_ID, nID );
	SetWindowText(strCaption);
	m_pView->m_Number = nID;

	SetWindowLongPtr( GetSafeHwnd(), GWL_ID, nID_Start + nID );
}

const int c_nLeft	= 10;
const int c_nRight	= 10;
const int c_nTop	= 10;
const int c_nBottom = 10;


CRect GetFloatAdjust()
{
	return CRect( -c_nLeft, -c_nTop, c_nRight, c_nBottom  );
}

CRect GetNormalAdjust(int nGap)
{
	return CRect( -3 , -3 - nGap  , 3  , 3 );
}

void CCameraBar::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp )
{
	CRect rc( /*IsFloating() ? GetFloatAdjust() :*/ GetNormalAdjust( nGapGripper ) );
	lpncsp->rgrc[0].left -= rc.left;
	lpncsp->rgrc[0].top -= rc.top;
	lpncsp->rgrc[0].right -= rc.right;
	lpncsp->rgrc[0].bottom -= rc.bottom;
}

BEGIN_MESSAGE_MAP(CCameraBar, CGuiControlBar)
	//{{AFX_MSG_MAP(CCameraBar)
	ON_WM_DESTROY()
	ON_WM_NCPAINT()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraBar message handlers

void CCameraBar::OnDestroy() 
{
	CGuiControlBar::OnDestroy();
	
	// TODO: Add your message handler code here
	CGuiControlBar::SaveBar( CString(_T("CameraBars") ));
}

CSize CCameraBar::CalcDynamicLayout(int nLength, DWORD nMode)
{
	m_bKeepSizeWhenDocked = TRUE;
	if (IsFloating())
	{
		// Enable diagonal arrow cursor for resizing
		//m_sizeVert=m_sizeHorz=CSize(200,200);
		GetParent()->GetParent()->ModifyStyle(MFS_4THICKFRAME|WS_CAPTION,0);
	} 


		SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
			SWP_NOACTIVATE | SWP_FRAMECHANGED|SWP_NOREDRAW);
		SendMessage(WM_NCPAINT);
	 	return CControlBar::CalcDynamicLayout(nLength, nMode);
	
/*	CSize sz =  CGuiControlBar::CalcDynamicLayout(  nLength, nMode);
	return sz;*/
}
/*
CSize CCameraBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
		CSize size = m_pView->GetBackground()->GetSize();


    if (bStretch) // the bar is stretched (is not the child of a dockbar)
        if (bHorz)
            return CSize(32767, size.cy);
        else
            return CSize(size.cx, 32767);
	else 
		return size;
}

*/

CSize CCameraBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

//	CGuiControlBar::CalcFixedLayout( bStretch, bHorz );
	return CalcLayout(dwMode);
}
/*
CSize CCameraBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}


//	CGuiControlBar::CalcDynamicLayout( dwMode, nLength );

	CSize sz =  CalcLayout(dwMode, nLength);
			return sz;
}*/

CSize CCameraBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);


	CSize sizeResult(0,0);

	//BLOCK: Load Buttons

	sizeResult += m_pView->GetBackground()->GetSize() ;

	if( IsFloating() )
	{
		int z = 0;
	}
	else 
	{
		int z = 0;
	}
	//BLOCK: Adjust Margins
	{
/*		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
//		sizeResult.cx -= rect.Width();

		CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);*/

		CRect rc( /*IsFloating() ? GetFloatAdjust() : */GetNormalAdjust( nGapGripper ) );
	/*	CSize sizeGap  ( c_nLeft + c_nRight, c_nBottom + c_nTop + (IsFloating() ? 0 : nGapGripper)   );
		sizeResult += sizeGap;*/

		sizeResult.cx += rc.Width();
		sizeResult.cy += rc.Height();
	}

	return sizeResult;
}

//***************** for pause button *******************************//

void CCameraBar::OnNcPaint()
{
	CGuiControlBar::OnNcPaint();
	CRect rcPause(m_rcCloseBtn);
	rcPause.OffsetRect( -(rcPause.Width() + 2), 0 );
	
  if (GetStyle() & WS_SYSMENU /*&& m_PauseButton.m_imgList*/)
  {
	  CWindowDC	dc(this);
	  if(!m_bActive)
	    m_PauseButton.Paint(&dc,m_statePause, rcPause,::GetSysColor(COLOR_BTNFACE));
	  else
	    m_PauseButton.Paint(&dc,m_statePause, rcPause,::GetSysColor(COLOR_ACTIVECAPTION));
  }

	
}

const UINT HTPAUSE = 10000;

void CCameraBar::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (m_statePause == NORMAL) 
	{
		if (nHitTest == HTPAUSE )
		{
			m_statePause = OVER;
			SetTimer(IDT_PauseButton,100,0);
		}
		SendMessage(WM_NCPAINT);
	}
	CGuiControlBar::OnNcMouseMove(nHitTest, point);
}

void CCameraBar::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (nHitTest == HTPAUSE)
	{ 
		m_statePause=PRESS;
		SendMessage(WM_NCPAINT);
		return;
	}

	CGuiControlBar::OnNcLButtonDown(nHitTest, point);
}

void CCameraBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rcPause(m_rcCloseBtn);
	rcPause.OffsetRect( -(rcPause.Width() + 2), 0 );

	ClientToScreen(rcPause);
	CPoint pt(point);
	pt.y+=23;
	pt.x+=5;
	if (rcPause.PtInRect(pt))
	{
		
		if (m_statePause ==PRESS)
		{
			m_statePause = NORMAL;
			KillTimer(IDT_PauseButton);
			OnPause();
		}
		SendMessage(WM_NCPAINT);
		return;
	}

	if( nHitTest == HTPAUSE) return;

	CGuiControlBar::OnNcLButtonUp(nHitTest, point);
}

void CCameraBar::OnTimer(UINT nIDEvent)
{
	if (m_statePause !=NORMAL) 
	{
		CPoint pt(GetMessagePos());
		
		CRect rcPause(m_rcCloseBtn);
		rcPause.OffsetRect( -(rcPause.Width() + 2), 0 );

		ClientToScreen(rcPause);
		pt.y+=23;
		pt.x+=5;
		if (!rcPause.PtInRect(pt))
		{
			m_statePause=NORMAL;
			KillTimer(IDT_PauseButton);
			SendMessage(WM_NCPAINT); 		
		}
	}

	CGuiControlBar::OnTimer(nIDEvent);
}

UINT CCameraBar::OnNcHitTest(CPoint point)
{
	CRect rcPause(m_rcCloseBtn);
	rcPause.OffsetRect( -(rcPause.Width() + 2), 0 );

	CPoint pt(point);
	ClientToScreen(rcPause);
	pt.y+=23;
	pt.x+=5;

	if( rcPause.PtInRect( pt )) 
	{
		return HTPAUSE;
	}

	return CGuiControlBar::OnNcHitTest(point);
}

void CCameraBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if( nHitTest == HTPAUSE )
	{
		return;
	}
	CGuiControlBar::OnNcLButtonDblClk(nHitTest, point);
}

void	CCameraBar::OnPause()
{
	CBackground* pBg = m_pView->GetBackground();
	CVideoBackground* pVBg = dynamic_cast<CVideoBackground*> ( pBg );
	CRealtimeBackground* pRBg = 0;
	if(pVBg)
	{
		OAFilterState fs = pVBg->GetState();
		fs == State_Running ? pVBg->Pause() : pVBg->Play();
	}
	else if( pRBg = dynamic_cast<CRealtimeBackground*> ( pBg ) )
	{
//NOTENOTE: Not currently tested
		pRBg->IsPlay() ? pRBg->Stop() : pRBg->Play();
	}
}

//*****************************************************************//

void	CCameraBar::OnCloseBtn(  )
{
	CWnd*	pFrame = (CWnd*)AfxGetMainWnd();
	int z = m_pView->m_Number;
	OutputDebugString(_T("CCamera::OnCloseBtn\n"));
	pFrame->PostMessage( WM_CLOSE_MASTER, z );
	CGuiControlBar::OnCloseBtn();
}
void CCameraBar::OnCancelMode() 
{
	CGuiControlBar::OnCancelMode();
	
	// TODO: Add your message handler code here
	m_pView->SendMessage(WM_CANCELMODE);
}
