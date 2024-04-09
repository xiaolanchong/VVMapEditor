// ExtentView.cpp : implementation file
//

#include "stdafx.h"
#include "VVMapEditor.h"
#include "ExtentView.h"
#include "VVMapEditorDoc.h"
#include "Color.h"

#include <limits>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef min
#undef max

const int IDT_HeightPoint	= 3;

/////////////////////////////////////////////////////////////////////////////
// CExtentView

IMPLEMENT_DYNCREATE(CExtentView, CScrollView)

CExtentView::CExtentView()
{
}

CExtentView::~CExtentView()
{
}


BEGIN_MESSAGE_MAP(CExtentView, CScrollView)
	//{{AFX_MSG_MAP(CExtentView)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtentView drawing

void CExtentView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	Update();
}

void CExtentView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	Graphics gr( pDC->GetSafeHdc() );
	gr.DrawImage( m_Bitmap.get(), 0, 0 );

	DrawHeight(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CExtentView diagnostics

#ifdef _DEBUG
void CExtentView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CExtentView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

CVVMapEditorDoc* CExtentView::GetDocument()
{
	return static_cast< CVVMapEditorDoc* >( m_pDocument );
}

/////////////////////////////////////////////////////////////////////////////
// CExtentView message handlers

void	CExtentView::Update()
{
	CVVMapEditorDoc* pDoc = GetDocument();

	m_Bitmap.release();
	const Array<long>& Arr = pDoc->m_ExtentTable[m_Index];
	if( !Arr.empty() )
	{
		m_Bitmap = std::unique_ptr<Bitmap>(new Bitmap( Arr.GetX(), Arr.GetY(), PixelFormat32bppARGB ));

/*		Graphics gr( m_Bitmap.get() );
		gr.Clear( Color::Red );*/

		long	Max = std::numeric_limits<long>::min(), 
				Min = std::numeric_limits<long>::max(); 

		for( int k = 0; k < Arr.GetX(); ++k )
			for( int l = 0; l < Arr.GetY(); ++l )
			{
				Max = (std::max<long>)( Max, Arr( k, l ) );
				Min = (std::min<long>)( Min, Arr( k, l ) );
			}

		BitmapData BmpData;

		int z = Arr.GetX();
		CSize size = GetSize();

		m_Bitmap->LockBits( &Rect(0, 0, size.cx, size.cy), ImageLockModeWrite , PixelFormat32bppARGB, &BmpData );
		
		Array2D<  Color > BmpArr( (Color*)BmpData.Scan0, BmpData.Width, BmpData.Height, BmpData.Stride );

		ASSERT( size.cx == BmpData.Width );
		ASSERT( size.cy == BmpData.Height );

		CColor	cl;
		cl.SetHLS(0.0f, 0.5f, 1.0f);

//#define _DUMP_EXTENT_
#ifdef _DUMP_EXTENT_
		CFile fileDump(_T("_dump_extent_bmp.txt"), CFile::modeCreate | CFile::modeWrite);
#endif _DUMP_EXTENT_

		 int Diff = Max - Min;
		 if( Diff < std::numeric_limits<float>::min()) Diff = std::numeric_limits<float>::min();
		for(int i = 0 ; i < size.cy; ++i)
		{
			for(int j = 0; j < size.cx; ++j)
			{
				float Hue = (( Arr( j, i ) - Min ) * 300.0f)/ Diff;
				Hue = (std::min)( 300.0f, Hue );
				Hue = (std::max)( 0.0f, Hue );
				cl.SetHue(Hue);
				COLORREF cr = cl;
				BmpArr(j, i) = Color( GetRValue(cr), GetGValue(cr), GetBValue(cr))  ;

			#ifdef _DUMP_EXTENT_
				CString str;
				str.Format(_T("%04x "), Arr( j, i ) );
				fileDump.Write(str, str.GetLength()  );
			#endif _DUMP_EXTENT_
			}
		#ifdef _DUMP_EXTENT_
			const TCHAR* z = "\n";
			fileDump.Write( z, sizeof(TCHAR));
		#endif _DUMP_EXTENT_			
		}

		m_Bitmap->UnlockBits(&BmpData);

		SetScrollSizes( MM_TEXT, size );
	}
}

CSize	CExtentView::GetSize()
{
	if( m_Bitmap.get() )
	{
		return CSize( m_Bitmap->GetWidth(), m_Bitmap->GetHeight() );
	}
	else
		return CSize(0,0);
}

void CExtentView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	Update();
	Invalidate();
}

void CExtentView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent()			&& 
		pDoc->IsCameraInCurrent(pDoc->IndexToId( m_Index))  &&
		pDoc->m_bShowHeights)
	{
		PointF CurPoint(point.x, point.y);
		int Index = m_Index;
		pDoc->m_HeightPoint = std::make_pair( Index, point );

		pDoc->DrawHeight();


		SetTimer( IDT_HeightPoint, 100, 0 );

	}

	CScrollView::OnMouseMove(nFlags, point);
}

void	CExtentView::DrawHeight(CDC* pDC)
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( pDoc->IndexToId( m_Index) ) &&
		pDoc->m_bShowHeights)
	{
		int Index = m_Index;
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

void	CExtentView::DrawHeight()
{
	CVVMapEditorDoc* pDoc = GetDocument();
	if( pDoc->IsCurrent() && 
		pDoc->IsCameraInCurrent( pDoc->IndexToId( m_Index) ) &&
		pDoc->m_bShowHeights)
	{
		int Index = m_Index;
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

void CExtentView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CVVMapEditorDoc* pDoc = GetDocument();
	if( nIDEvent == IDT_HeightPoint )
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
