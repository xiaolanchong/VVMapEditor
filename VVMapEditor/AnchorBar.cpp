// AnchorBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "AnchorBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnchorBar

CAnchorBar::CAnchorBar()
{
	m_sizeMinFloating=m_sizeVert=m_sizeHorz= m_sizeHorzt = m_sizeVertt = CSize(250,163);
	m_pwndView		= new CAnchorView;
	m_pwndMapScale	= (CMapScaleView*)RUNTIME_CLASS(CMapScaleView)->CreateObject();
}

CAnchorBar::~CAnchorBar()
{
/*	if(m_pwndView)
		delete m_pwndView;*/


}


BEGIN_MESSAGE_MAP(CAnchorBar, CGuiControlBar)
	//{{AFX_MSG_MAP(CAnchorBar)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnchorBar message handlers

BOOL	CAnchorBar::Create(CWnd* pParentWnd)
{
	CString strSPCaption; strSPCaption.LoadString( IDS_SUPPORT_POINTS );
	if( !CGuiControlBar::Create(strSPCaption, WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, 0x200 ))
	{
		TRACE0( "Failed to create Anchor bar" );
		return FALSE;
	}

//	BOOL res = m_wndDialog.Create( IDD_ANCHOR, this );
//	m_wndDialog.ShowWindow(SW_SHOW);

		CFrameWnd*	pFrame = (CFrameWnd*)/*AfxGetMainWnd()*/pParentWnd;

	CCreateContext cc;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CDocument* pDoc = pTmpl->GetNextDoc(pos);
	cc.m_pCurrentDoc = pDoc;
	cc.m_pCurrentFrame = pFrame;
	cc.m_pLastView = pFrame->GetActiveView();
	cc.m_pNewViewClass = RUNTIME_CLASS(CAnchorView);

	if (!m_wndTab.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;

	m_pwndView->Create( 0, 0, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), /*this*/&m_wndTab, 10 , &cc);
	cc.m_pNewViewClass = RUNTIME_CLASS(CMapScaleView);
	m_pwndMapScale->Create( 0, 0,  WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), &m_wndTab, 11 , &cc);

	m_wndTab.SetImageList( IDB_CONTROL_ANCHOR, 16, 10, RGB (192, 192, 192));
	m_wndTab.Addtab(m_pwndView, strSPCaption, 0);
	SetSupporMultiView();

	CString strRPCaption;
	strRPCaption.LoadString( IDS_REFERENCE_POINTS );

	m_wndTab.Addtab(m_pwndMapScale, strRPCaption, 1);


	return TRUE;
}


void CAnchorBar::OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler )
{
	m_pwndView->OnUpdateCmdUI();
	m_pwndMapScale->OnUpdateCmdUI();
}