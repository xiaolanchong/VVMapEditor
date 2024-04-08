// CompileBar.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "CompileBar.h"
#include "GuiDrawLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompileBar

CCompileBar::CCompileBar() : m_fProgress(0.0)
{
}

CCompileBar::~CCompileBar()
{
}


BEGIN_MESSAGE_MAP(CCompileBar, CGuiStatusBar)
	//{{AFX_MSG_MAP(CCompileBar)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompileBar message handlers

BOOL CCompileBar::CreateCompileBar(UINT nCompilePane)
{
	UINT nID, nStyle;
	int nWidth;
	GetPaneInfo(nCompilePane, nID, nStyle, nWidth);
	SetPaneInfo(nCompilePane, nID, nStyle, 150);
	SetPaneText(nCompilePane, _T(""));

	return TRUE;
}

void	CCompileBar::ResetProgress()
{
	m_fProgress = 0.0f;
	Invalidate();
}

void	CCompileBar::SetProgress( float fPart )
{
	m_fProgress = fPart;
	Invalidate();
}

void CCompileBar::OnPaint() 
{
//	CGuiStatusBar::OnPaint();
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CGuiStatusBar::OnPaint() for painting messages
	CRect rcCli;
	CRect rcClient,rcTemp;
	CBrush cb;
	GetClientRect(&rcClient);
	cb.CreateSolidBrush(GuiDrawLayer::GetRGBColorFace());
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)dc.m_hDC, 0 );
	rcTemp=rcClient;
	rcClient.bottom=rcClient.top+2;
	rcClient.right+=50;
	dc.FillRect(rcClient,&cb);
	Drawpanels(&dc);
	cb.DeleteObject();

	const int c_nCompilePane = 3;
	GetItemRect( c_nCompilePane ,&rcCli);


	if( !rcCli.IsRectEmpty() )
	{
		rcCli.DeflateRect(2,2);
		CRect rcText(rcCli);
		rcCli.right = rcCli.left + long(rcCli.Width() * m_fProgress);

		dc.FillSolidRect(&rcCli, GuiDrawLayer::GetRGBCaptionXP());

		if( m_fProgress != 0.0f )
		{
		CString strProgress;
		strProgress.Format( _T("%d%%"), DWORD(m_fProgress*100) );
		dc.SetBkMode( TRANSPARENT ); 
		dc.DrawText( strProgress, &rcText, DT_SINGLELINE|DT_CENTER | DT_VCENTER  );
		}
	}
}
