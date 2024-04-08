// PointList.cpp: implementation of the PointList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VVMapEditor.h"
#include "PointList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PointList::PointList()
{

}

PointList& PointList::operator =  (const PointList& pl)
{
	if( &pl != this )
	{
//		m_bMap = (pl.m_bMap);
		m_MapPoints = (pl.m_MapPoints);
//		m_MapPolygons = (pl.m_MapPolygons);
		m_Points = (pl.m_Points);
//		m_Polygons = (pl.m_Polygons);
	}
	return *this;
}