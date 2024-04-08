// StateCache.cpp: implementation of the CStateCache class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "vvmapeditor.h"
#include "StateCache.h"
#include "res/resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

boost::shared_ptr<CImageList> CStateCache::s_ilState;

const UINT c_nXSize = 16;
const UINT c_nYSize = 16;

CStateCache::CStateCache() : m_bValid(false)
{
	if( ! s_ilState.get() )
	{
		s_ilState = boost::shared_ptr<CImageList>( new CImageList );
		s_ilState->Create( IDB_GETXY , c_nXSize, 1, RGB(192,192,192) );
	}
}

CStateCache::CStateCache(	CPoint	point, long	err, CWnd*	pWnd) : 
	m_Point(point), m_lError(err), m_bValid(false)
{
	CDC* pDC = pWnd->GetDC();

	m_pOldBitmap = boost::shared_ptr<CBitmap>( new CBitmap );
	m_pOldBitmap->CreateCompatibleBitmap( pDC, c_nXSize, c_nYSize );
	CDC	 MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject( m_pOldBitmap.get() );
	MemDC.BitBlt( 0, 0, c_nXSize, c_nYSize, pDC, point.x, point.y, SRCCOPY );
	pWnd->ReleaseDC( pDC );
}

void	CStateCache::Draw(CDC* pDC)
{
	if( m_pOldBitmap.get()  )
	{
	
	int nImage = -1 ;
	switch(m_lError)
	{
		case  ERR_NO_ERROR				: nImage = 0; break;
		case  ERR_TRANSITION_UNDEFINED	: nImage = 1; break;
		case  ERR_MAP_NOT_COMPILED		: nImage = 2; break;
		case  ERR_TRANSITION_INFINITY	: nImage = 3; break;
		case  ERR_POINT_INVISIBLE_SRC	: nImage = 4; break;
		case  ERR_POINT_INVISIBLE		: nImage = 5; break;
		case  ERR_POINT_OUT_OF_RECT_SRC : nImage = 6; break; 
		case  ERR_POINT_OUT_OF_RECT		: nImage = 7; break; 
		case  ERR_INVALID_MASTERS	: nImage = 8; break;
		case  ERR_CRITSECTION_OCCUPIED : nImage = 8; break;
	}
	if( nImage >= 0 )
	s_ilState->Draw( pDC, nImage, m_Point, ILD_TRANSPARENT );
	m_bValid = true;
	}
}

void	CStateCache::DrawInvert(CDC* pDC)
{
	if( m_bValid && m_pOldBitmap.get()  )
	{
	
	CDC	 MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject( m_pOldBitmap.get() );

		CPoint pt = m_Point;
		pt.Offset( m_sizeScroll );
	pDC->BitBlt( m_Point.x, m_Point.y, 16, 16, &MemDC, 0, 0, SRCCOPY );
	m_bValid = false;
	}
}

void	CStateCache::Invalidate(CWnd* pWnd)
{

}

void	CStateCache::Draw(CWnd* pWnd)
{
	m_sizeScroll = CSize( pWnd->GetScrollPos( SB_HORZ ), pWnd->GetScrollPos(SB_VERT) );
	CDC* pDC = pWnd->GetDC();
	Draw(pDC);
	pWnd->ReleaseDC( pDC );
}

void	CStateCache::DrawInvert(CWnd* pWnd)
{
	CSize sizeScroll = CSize( pWnd->GetScrollPos( SB_HORZ ), pWnd->GetScrollPos(SB_VERT) );
	m_Point.Offset( sizeScroll - m_sizeScroll );
	CDC* pDC = pWnd->GetDC();
	DrawInvert(pDC);
	pWnd->ReleaseDC( pDC );
}