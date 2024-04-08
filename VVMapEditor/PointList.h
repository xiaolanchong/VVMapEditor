// PointList.h: interface for the PointList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTLIST_H__52125880_89A5_4B2D_9B33_F37BF163170A__INCLUDED_)
#define AFX_POINTLIST_H__52125880_89A5_4B2D_9B33_F37BF163170A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<vector>
#include	<map>
#include	<set>

#include "RectF.h"
#include "Array2D.h"

typedef std::vector< std::pair< bool, PointF > >	PointArr_t;
typedef std::vector< PointArr_t >					PointMap_t;
typedef std::vector< PointF >						Polygon_t;
typedef std::vector< Polygon_t>						PolygonArr_t;
typedef std::vector< PolygonArr_t >					PolygonMap_t;

//typedef std::pair< Polygon_t, int >					InvZone_t;

typedef std::vector< std::vector<CRectF> > 			ExtentMap_t;

typedef std::vector< Array<long>	>				ExtentTable_t;

struct PointList  
{
public:
	PointList();
	PointList(const PointList& pl):
		m_MapPoints(pl.m_MapPoints),
		m_Points(pl.m_Points)
	{
	}

	PointList& operator =  (const PointList& pl);

	void Clear() { m_MapPoints.clear(); m_Points.clear(); }

	PointArr_t											m_MapPoints;
	
	
	PointMap_t											m_Points;			
};

//-----------------------------------------------------------------------------
// Purpose: describe invisible zone on map
//-----------------------------------------------------------------------------
struct	InvZone_t
{
	enum
	{
		ERROR_NUMBER = -1
	};
	Polygon_t		m_Polygon;
	int				m_nZoneNumber;
	int				m_nMasterNumber;

	InvZone_t() : m_Polygon(), m_nZoneNumber(ERROR_NUMBER), m_nMasterNumber(ERROR_NUMBER)
	{}
	InvZone_t(const InvZone_t& iz) : 
		m_Polygon(iz.m_Polygon), 
		m_nZoneNumber(iz.m_nZoneNumber),
		m_nMasterNumber( iz.m_nMasterNumber ) 
	{}
	InvZone_t(const Polygon_t& pol, int nZoneNumber, int nMasterNumber) : 
		m_Polygon(pol), 
		m_nZoneNumber(nZoneNumber),
		m_nMasterNumber( nMasterNumber ) 
	{}
	InvZone_t& operator = ( const InvZone_t& iz)
	{
		if( this != & iz)
		{
			m_Polygon		=	iz.m_Polygon; 
			m_nZoneNumber	=	iz.m_nZoneNumber;
			m_nMasterNumber =	iz.m_nMasterNumber;
		}
		return *this;
	}
};

typedef std::vector< InvZone_t > 			InvZoneMap_t;

struct	AnchorPoint_t
{
	bool	m_bOnMap;
	long	m_MapX, m_MapY;
public:
	//NOTENOTE: default copy constructor & operator =
	enum
	{
		NO_DEPENDENCE = -1
	};
	int		m_Relative;
	double	m_x, m_y, m_z;


	AnchorPoint_t( int Relative = NO_DEPENDENCE) : 
		 m_Relative(Relative), m_bOnMap(false)
		{}

	AnchorPoint_t( double x, double y , double z, int Relative = NO_DEPENDENCE) : 
		m_Relative(Relative),
		m_x(x), m_y(y), m_z(z),
		m_bOnMap( false ),
		m_MapX(), m_MapY()
		{}

	void	SetMapCoord( long x, long y ) 
	{
		m_MapX		= x; 
		m_MapY		= y;
		m_bOnMap	= true;
	}

	void					RemoveFromMap()			{ m_bOnMap	= false; }
	bool					IsOnMap()		const	{ return m_bOnMap;	}
	std::pair<long ,long>	GetMapCoord()	const	{ return std::make_pair( m_MapX, m_MapY ); }
};

typedef		std::vector<AnchorPoint_t>		AnchorPointMap_t;

typedef		std::vector< std::pair<int, double>  >	RefPoint_t;
typedef		std::vector< RefPoint_t>				RefPointMap_t;

#endif // !defined(AFX_POINTLIST_H__52125880_89A5_4B2D_9B33_F37BF163170A__INCLUDED_)
