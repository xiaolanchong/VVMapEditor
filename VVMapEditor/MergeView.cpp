// MergeView.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "MergeView.h"

#include "VVMapEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeView

IMPLEMENT_DYNCREATE(CMergeView, CScrollView)

CMergeView::CMergeView():m_Bmp(0), m_Index(-1)
{
}

CMergeView::~CMergeView()
{
	if(m_Bmp) ::delete m_Bmp;
}


BEGIN_MESSAGE_MAP(CMergeView, CScrollView)
	//{{AFX_MSG_MAP(CMergeView)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeView drawing

void CMergeView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CVVMapEditorDoc* pDoc = static_cast<CVVMapEditorDoc*>(GetDocument());
	m_rcArea =	pDoc->GetMergeRect( m_Index );

	//NOTENOTE: create on initial update instead OnTimer
	Bitmap *Bmp = pDoc->CreateBitmap( m_Index, m_rcArea );
	SetBitmap( Bmp );

//	SetTimer( 0, 250 , 0);
}


CSize	CMergeView::CreateBitmap()
{
	CVVMapEditorDoc* pDoc = static_cast<CVVMapEditorDoc*>(GetDocument());
	Bitmap *Bmp = pDoc->CreateBitmap( m_Index, m_rcArea );
	SetBitmap( Bmp );
	return Bmp? CSize( Bmp->GetWidth(), Bmp->GetHeight() ) : CSize(0,0);
}

void CMergeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	Graphics gr(pDC->GetSafeHdc());

	if(m_Bmp) gr.DrawImage( m_Bmp, 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////
// CMergeView diagnostics

#ifdef _DEBUG
void CMergeView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMergeView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeView message handlers

BOOL CMergeView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CScrollView::OnEraseBkgnd(pDC);
}

void CMergeView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//NOTENOTE: don't update bitmap, it may be too slow, comment out if it's need
//	Invalidate(FALSE);


	CScrollView::OnTimer(nIDEvent);
}

void CMergeView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void	CMergeView::SetBitmap(Bitmap* bmp) 
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
