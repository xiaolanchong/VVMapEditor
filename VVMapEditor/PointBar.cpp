// PointBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "PointBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointBar

CPointBar::CPointBar()
{
}

CPointBar::~CPointBar()
{
}

const int nID_Start = 1;

BOOL	CPointBar::Create( CWnd* pParentWnd )
{
	CString strCaption;
	strCaption.LoadString( IDS_POINTS );
	baseControlBar::Create( strCaption, WS_CLIPCHILDREN| WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, nID_Start );
/*
	baseControlBar::Create( strCaption, pParentWnd, CSize(200, 200), TRUE, nID_Start, WS_CLIPCHILDREN| WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU );
*/	
	if (!m_wndTab.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;

	m_wndPoint.Create( WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDRAWFIXED , CRect(0,0,200,500), &m_wndTab, 1, 0 );
	m_wndPoint.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT);
	m_wndExtent.Create( WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDRAWFIXED , CRect(0,0,200,500), &m_wndTab, 1, 0 );
	m_wndExtent.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT);

	m_wndTab.SetImageList( IDB_CONTROL_POINT, 16, 20, RGB (192, 192, 192));
	strCaption.LoadString( IDS_POINTS );
	m_wndTab.Addtab(&m_wndPoint, strCaption, 0);
	strCaption.LoadString( IDS_EXTENTS );
	m_wndTab.Addtab(&m_wndExtent, strCaption, 1);
	LV_COLUMN lvc;
	strCaption.LoadString( IDS_POINTS );
	memset(&lvc,0,sizeof(lvc));
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 100;
	lvc.pszText = const_cast<LPTSTR> (  static_cast<LPCTSTR> (strCaption ));
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	m_wndPoint.InsertColumn(0,&lvc);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPointBar, baseControlBar)
	//{{AFX_MSG_MAP(CPointBar)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointBar message handlers

void CPointBar::OnDestroy() 
{
	baseControlBar::OnDestroy();
	
	// TODO: Add your message handler code here
//	SaveBar( CString(_T("PointBar") ));
}

void CPointBar::OnSize(UINT nType, int cx, int cy) 
{
	baseControlBar::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
/*	if( m_wndPoint.GetSafeHwnd() )
		m_wndPoint.SetWindowPos(0, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER);*/
}
