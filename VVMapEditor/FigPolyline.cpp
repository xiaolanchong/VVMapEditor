// FigPolyline.cpp: implementation of the CFigPolyline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "FigPolyline.h"
#include "Zoom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFigPolyline::CFigPolyline(PointF point) : m_EndPoint( point ) , m_IsEndPointValid(true)
{
	m_Points.reserve(10);
	m_Points.push_back(point);
//	m_Points.push_back(point);
}

CFigPolyline::~CFigPolyline()
{

}

/*
void CFigPolyline::AddPoint(PointF point)
{
	m_Points.push_back( point );
}

void CFigPolyline::SetEndPoint(PointF point)
{
	m_Points.back() = point;
}

void CFigPolyline::OffsetEndPoint(SizeF size)
{
	m_Points.back().X += size.Width;
	m_Points.back().Y += size.Height;
}*/

void	CFigPolyline::LButtonDown	( PointF point )
{
//	m_Points.push_back( point);
	m_EndPoint = point;
	m_IsEndPointValid = true;
}

void	CFigPolyline::LButtonUp	( PointF point )
{
	m_Points.push_back( point);
	m_IsEndPointValid = false;
}

void	CFigPolyline::LButtonDbl	( PointF point )
{
}

void	CFigPolyline::MouseMove	( PointF point )
{
	if( m_IsEndPointValid ) m_EndPoint = point;
}


void CFigPolyline::Draw(Graphics& gr, const Zoom& zoom, bool bCurrent) const 
{
	CFigPolyline::Draw(gr, zoom, m_Points);
}

void CFigPolyline::DrawEdit(CWnd* pWnd, const Zoom& zoom) const 
{
	CDC* pDC = pWnd->GetDC();
	std::vector<CPoint> ptz;
	ptz.reserve( m_Points.size() );
	CSize sizeScroll = zoom.Offset;
	for (int i=0; i < m_Points.size(); ++  i)
	{
		CPoint pt ( zoom.ScaleX * m_Points[i].X, zoom.ScaleY * m_Points[i].Y );
		pt.Offset( sizeScroll );
		ptz.push_back(pt);
	}
	if( m_IsEndPointValid )
	{
		CPoint pt ( zoom.ScaleX * m_EndPoint.X, zoom.ScaleY * m_EndPoint.Y );
		pt.Offset( sizeScroll );
		ptz.push_back(pt);
	}
	int nOldMode = pDC->SetROP2(R2_NOT);
	if( ptz.size() == 2 )
	{
		pDC->MoveTo( ptz[0] );
		pDC->LineTo( ptz[1] );
	}
	else
		pDC->Polygon( &ptz[0], ptz.size() );
	pDC->SetROP2( nOldMode );
	pWnd->ReleaseDC(pDC);
}

void CFigPolyline::InvalidateEdit(CWnd* pWnd, const Zoom& zoom) const 
{
	DrawEdit( pWnd, zoom );
}

void	CFigPolyline::Draw(Graphics& gr, const Zoom& zoom, const std::vector<PointF>& Points)
{

}