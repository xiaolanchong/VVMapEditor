// CrossCtrl.cpp : implementation file
//

#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "CrossCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrossCtrl

CCrossCtrl::CCrossCtrl() : m_dwExtent( 20 ), m_dwThickness(1)
{
}

CCrossCtrl::~CCrossCtrl()
{
}


BEGIN_MESSAGE_MAP(CCrossCtrl, CStatic)
	//{{AFX_MSG_MAP(CCrossCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrossCtrl message handlers

void CCrossCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages

	CRect rcClient;
	GetClientRect( &rcClient );
	CDC MemDC;
	CBitmap	MemBmp;
	MemDC.CreateCompatibleDC( &dc );
	MemBmp.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	CGdiObject* pOldBmp = MemDC.SelectObject( &MemBmp );
	MemDC.FillSolidRect( &rcClient, GetSysColor( COLOR_3DFACE ) );
	CPoint ptCenter = rcClient.CenterPoint();
	MemDC.PatBlt( ptCenter.x - m_dwThickness/2, ptCenter.y - m_dwExtent/2,
				  m_dwThickness, m_dwExtent, BLACKNESS );
	MemDC.PatBlt( ptCenter.x - m_dwExtent/2, ptCenter.y - m_dwThickness/2,
				  m_dwExtent, m_dwThickness, BLACKNESS );	

	dc.BitBlt( 0, 0, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, SRCCOPY );
	MemDC.SelectObject( pOldBmp );
}
