// StateCache.h: interface for the CStateCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATECACHE_H__633AE23A_1F5B_43A3_A824_6BA86D331A9A__INCLUDED_)
#define AFX_STATECACHE_H__633AE23A_1F5B_43A3_A824_6BA86D331A9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/shared_ptr.hpp>

struct CStateCache  
{
	CSize									m_sizeScroll;
	bool									m_bValid;

	CPoint									m_Point;
	long									m_lError;
	static	boost::shared_ptr<CImageList>	s_ilState;
	boost::shared_ptr<CBitmap>			m_pOldBitmap;

	CStateCache();
	CStateCache(	CPoint	point,	// center
					long	err,
					CWnd*	pWnd
				) ;
	CStateCache(const CStateCache& rhs):
				m_Point(rhs.m_Point), 
				m_lError(rhs.m_lError),
				m_pOldBitmap(rhs.m_pOldBitmap)
				{};

	CStateCache& operator =(const CStateCache& rhs)
	{
		if(&rhs != this)
		{
			m_Point		= rhs.m_Point;
			m_lError	= rhs.m_lError;
			m_pOldBitmap= rhs.m_pOldBitmap;
		}
		return *this;
	}

	void	Offset( CSize size)
	{
//		m_Point += size;
	}

	void	Draw(CDC* pDC);
	void	DrawInvert(CDC* pDC);

	void	Draw(CWnd* pWnd);
	void	DrawInvert(CWnd* pWnd);
	
	void	Invalidate(CWnd* pWnd);
	void	Invalidate() {  m_bValid = false;}
};

#endif // !defined(AFX_STATECACHE_H__633AE23A_1F5B_43A3_A824_6BA86D331A9A__INCLUDED_)
