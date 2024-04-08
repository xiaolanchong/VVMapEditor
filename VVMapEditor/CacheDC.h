// CacheDC.h: interface for the CCacheDC class.
//
//////////////////////////////////////////////////////////////////////

/************************************
  Revised on 25.08.2004 18:17:11
  Comments: Добавлено отображение для высот
 ************************************/


#if !defined(AFX_CACHEDC_H__12BC412A_D049_4FAE_A8FC_EA835FDED6C3__INCLUDED_)
#define AFX_CACHEDC_H__12BC412A_D049_4FAE_A8FC_EA835FDED6C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template<typename T> class CCacheDC
{
	bool	m_bValidCache;
	T		m_Cache;
	CSize	m_ScrollPos;
public:
	CCacheDC();

	void	Draw(CDC *	pDC, const T& NewCache);
	void	Draw(CWnd*	pWnd, const T& NewCache);
	T&		GetCache() { return m_Cache;}
	void	InvalidateCache(CWnd* pWnd);
};

template<typename T> CCacheDC<T>::CCacheDC(): 
	m_bValidCache(false), 
	m_Cache()
{
	
}

template<typename T> void CCacheDC<T>::Draw(CDC * pDC, const T& NewCache)
{
	m_bValidCache = true;
	m_Cache = NewCache;
	m_Cache.Draw(pDC);
	m_ScrollPos = CSize(0,0);
}

template<typename T> void CCacheDC<T>::Draw(CWnd * pWnd, const T& NewCache)
{
	CSize sizeScroll (	pWnd->GetScrollPos( SB_HORZ ),
						pWnd->GetScrollPos( SB_VERT ) );

	CDC* pDC = pWnd->GetDC();
	if(m_bValidCache)
	{
		CSize sizeOff = sizeScroll - m_ScrollPos;
		m_Cache.Offset( -sizeOff );
		m_Cache.DrawInvert(pDC);
	}
	m_bValidCache = true;
	m_Cache = NewCache;
	m_Cache.Offset( -sizeScroll );
	m_ScrollPos = sizeScroll;
	m_Cache.Draw(pDC);
	pWnd->ReleaseDC(pDC);
}
/*
template<typename T> void CCacheDC<T>::DrawUncached(CWnd * pWnd, const T& NewCache)
{
	CSize sizeScroll (	pWnd->GetScrollPos( SB_HORZ ),
						pWnd->GetScrollPos( SB_VERT ) );

	CDC* pDC = pWnd->GetDC();
	m_bValidCache = true;
	m_Cache = NewCache;
	m_Cache.Offset( -sizeScroll );
	m_ScrollPos = sizeScroll;
	m_Cache.Draw(pDC);
	pWnd->ReleaseDC(pDC);
}
*/
template<typename T> void CCacheDC<T>::InvalidateCache(CWnd * pWnd)
{
	if(m_bValidCache)
	{
		CSize sizeScroll (	pWnd->GetScrollPos( SB_HORZ ),
							pWnd->GetScrollPos( SB_VERT ) );

		CSize sizeOff = sizeScroll - m_ScrollPos;
		m_Cache.Offset( sizeOff );
		m_ScrollPos = sizeScroll;
		CDC* pDC = pWnd->GetDC();
		m_Cache.DrawInvert(pDC);
		m_bValidCache = false;
		pWnd->ReleaseDC(pDC);
	}
}

struct CCrossCache
{
	CPoint	m_Point;
	bool	m_bVisible;
	int		m_nExtent;
	int		m_nThickness;

	CCrossCache(){};
	CCrossCache(	CPoint	point,	// center
					bool	bVisible, // vision of point from getxy
					int		nExtent,  // width & height
					int		nThickness
				) : 
				m_Point(point), m_bVisible(bVisible), m_nExtent(nExtent), m_nThickness(nThickness) {};
	CCrossCache(const CCrossCache& rhs):
				m_Point(rhs.m_Point), m_bVisible(rhs.m_bVisible), m_nExtent(rhs.m_nExtent), m_nThickness(rhs.m_nThickness) {};

	CCrossCache& operator =(const CCrossCache& rhs)
	{
		if(&rhs != this)
		{
			m_Point		= rhs.m_Point;
			m_bVisible	= rhs.m_bVisible;
			m_nExtent	= rhs.m_nExtent;
			m_nThickness= rhs.m_nThickness;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
		m_Point += size;
	}

	void	Draw(CDC* pDC)
	{
		CPen pen( PS_SOLID, m_nThickness , RGB(0 , 96, 192) );
		CGdiObject* pOldPen = pDC->SelectObject( &pen );
		int nROP = pDC->SetROP2(R2_NOTXORPEN);
		if( m_bVisible )
		{
			pDC->MoveTo( m_Point.x - m_nExtent	, m_Point.y		);
			pDC->LineTo( m_Point.x + m_nExtent	, m_Point.y		);
			pDC->MoveTo( m_Point.x		, m_Point.y - m_nExtent );
			pDC->LineTo( m_Point.x		, m_Point.y + m_nExtent );
		}
		else
		{
			const double sqrt2_on_2 = 1.4142135/2;
			int Diag = int( m_nExtent * sqrt2_on_2);
			CGdiObject* pOldBrush = pDC->SelectStockObject( NULL_BRUSH );
			pDC->Ellipse(	m_Point.x - m_nExtent	, m_Point.y - m_nExtent,
							m_Point.x + m_nExtent	, m_Point.y + m_nExtent );
			pDC->MoveTo(	m_Point.x - Diag	, m_Point.y - Diag);
			pDC->LineTo(    m_Point.x + Diag	, m_Point.y + Diag );
			pDC->SelectObject( pOldBrush );
		}
		pDC->SetROP2(nROP);
		pDC->SelectObject(pOldPen);
	}

	void	DrawInvert(CDC* pDC)
	{
		Draw(pDC);
	}	
};

struct CPointCache
{
	CPoint	 m_Point;

	CPointCache(){};
	CPointCache(CPoint point) : m_Point(point) {};
	CPointCache(const CPointCache& rhs):m_Point(rhs.m_Point){}

	CPointCache& operator =(const CPointCache& rhs)
	{
		if(&rhs != this)
		{
			m_Point = rhs.m_Point;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
		m_Point += size;
	}

	void	Draw(CDC* pDC)
	{
//		CPen pen( PS_SOLID, 1 , RGB(0 , 96, 192) );
//		CGdiObject* pOldPen = pDC->SelectObject( &pen );
		int nROP = pDC->SetROP2(R2_NOT);
		pDC->Rectangle( m_Point.x - 4, m_Point.y - 4, m_Point.x + 1, m_Point.y + 1 );
		pDC->SetROP2(nROP);
//		pDC->SelectObject(pOldPen);
	}

	void	DrawInvert(CDC* pDC)
	{
		Draw(pDC);
	}	
};

typedef CCacheDC<CCrossCache>	CCrossDC;

struct CExtentCache
{
	CRect	 m_rcExtent;

	CExtentCache(){};
	CExtentCache(const CRect& point) : m_rcExtent(point) {};
	CExtentCache(const CExtentCache& rhs):m_rcExtent(rhs.m_rcExtent){}

	CExtentCache& operator =(const CExtentCache& rhs)
	{
		if(&rhs != this)
		{
			m_rcExtent = rhs.m_rcExtent;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
		m_rcExtent.OffsetRect(size);
	}

	void	Draw(CDC* pDC)
	{
		CPen pen(PS_SOLID, 1, RGB(255,0,0));
		CGdiObject* pOldBrush	= pDC->SelectStockObject(NULL_BRUSH);
		CGdiObject* pOldPen		= pDC->SelectObject(&pen);

		int nROP = pDC->SetROP2(R2_NOT);
		pDC->Rectangle( m_rcExtent );
		pDC->SetROP2(nROP);

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	void	DrawInvert(CDC* pDC)
	{
		Draw(pDC);
	}	
};

struct CAnchorCache
{
	CPoint	m_ptCenter;
	int		m_nWidth;

	CAnchorCache(){};
	CAnchorCache(CPoint point, int Width) : m_ptCenter(point), m_nWidth(Width) {};
	CAnchorCache(const CAnchorCache& rhs):m_ptCenter(rhs.m_ptCenter), m_nWidth(rhs.m_nWidth){}

	CAnchorCache& operator =(const CAnchorCache& rhs)
	{
		if(&rhs != this)
		{
			m_ptCenter = rhs.m_ptCenter;
			m_nWidth = rhs.m_nWidth;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
		m_ptCenter.Offset(size);
	}

	void	Draw(CDC* pDC)
	{
		CPen pen(PS_SOLID, 2, RGB(255,0,0));
		CGdiObject* pOldBrush	= pDC->SelectStockObject(NULL_BRUSH);
		CGdiObject* pOldPen		= pDC->SelectObject(&pen);

		int nROP = pDC->SetROP2(R2_NOT);
		pDC->MoveTo( m_ptCenter.x - m_nWidth, m_ptCenter.y - m_nWidth);
		pDC->LineTo( m_ptCenter.x + m_nWidth, m_ptCenter.y + m_nWidth);
		pDC->MoveTo( m_ptCenter.x - m_nWidth, m_ptCenter.y + m_nWidth);
		pDC->LineTo( m_ptCenter.x + m_nWidth, m_ptCenter.y - m_nWidth);

		pDC->SetROP2(nROP);

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	void	DrawInvert(CDC* pDC)
	{
		Draw(pDC);
	}
	
};

//typedef CCacheDC<CCrossCache>	CExtentDC;

#endif // !defined(AFX_CACHEDC_H__12BC412A_D049_4FAE_A8FC_EA835FDED6C3__INCLUDED_)
