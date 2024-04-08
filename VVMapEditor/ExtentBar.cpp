// ExtentBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "ExtentBar.h"
#include "VVMapEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtentBar

CExtentBar::CExtentBar(CWnd * pParentWnd, CString strName, int Index)
{
	CString sTitle;
	CGuiControlBar::Create( strName, WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | WS_SYSMENU, pParentWnd, Index + 0x100 );


/*
	CRect rect;
	GetClientRect(&rect);*/

	CCreateContext cc;
	memset( &cc, 0, sizeof(cc));

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pTmpl = AfxGetApp()->GetNextDocTemplate(pos);
	pos = pTmpl->GetFirstDocPosition();
	CDocument* pDoc = pTmpl->GetNextDoc(pos);
	m_pView = (CExtentView*)RUNTIME_CLASS(CExtentView)->CreateObject();
	m_pView->m_Index = Index;
	m_pView->Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER,CRect(0,0,0,0), this,0, &cc);
	pDoc->AddView( m_pView );
	m_pView->SendMessage(WM_INITIALUPDATE);

	CSize size = m_pView->GetSize();
	SetWindowPos(0, 0, 0, size.cx, size.cy, SWP_NOMOVE|SWP_NOZORDER);

//FIXME: Add window area defining code
	size.cx += 15;
	size.cy += 15 + 20;

	m_dwAllowDockingState = CBRS_ALIGN_ANY;

	m_sizeMinFloating = m_sizeHorzt = m_sizeVertt = size;
}

CExtentBar::~CExtentBar()
{
}


BEGIN_MESSAGE_MAP(CExtentBar, CGuiControlBar)
	//{{AFX_MSG_MAP(CExtentBar)
	ON_WM_NCRBUTTONDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_CB_ALLOW_DOCKING, OnAllowDocking)
	ON_COMMAND(ID_CB_CLOSE, OnCloseB)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtentBar message handlers

void CExtentBar::OnNcRButtonDown(UINT nHitTest, CPoint point) 
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

void CExtentBar::OnAllowDocking()
{
	CGuiControlBar::OnAllowDocking();	
}

void CExtentBar::OnCloseB()
{
	OnCloseBtn();	
}


void	CExtentBar::OnCloseBtn()
{
	CWnd*	pFrame = (CWnd*)AfxGetMainWnd();
	CVVMapEditorDoc * pDoc = (CVVMapEditorDoc*)m_pView->GetDocument();	
	pDoc->RemoveView(m_pView);
	int z = m_pView->m_Index;
	m_pView->DestroyWindow();
	OutputDebugString(_T("CMergeBar::OnCloseBtn\n"));
	pFrame->PostMessage( WM_CLOSE_EXTENT, z );
//	delete m_pView;
}

void CExtentBar::OnDestroy() 
{
	CGuiControlBar::OnDestroy();
	
	// TODO: Add your message handler code here
/*	
	CVVMapEditorDoc * pDoc = (CVVMapEditorDoc*)m_pView->GetDocument();	
	pDoc->RemoveView(m_pView);
	m_pView->DestroyWindow();*/
}
