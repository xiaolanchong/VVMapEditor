//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: Zoom is helper class for scaling floating points
//
//=============================================================================

#if !defined(AFX_ZOOM_H__D2705A11_40F9_48BC_AA61_F989DDEEEDEA__INCLUDED_)
#define AFX_ZOOM_H__D2705A11_40F9_48BC_AA61_F989DDEEEDEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RectF.h"

struct Zoom
{
	float	ScaleX ,ScaleY;
	CSize	Offset;
	
	Zoom() : ScaleX(1.0f), ScaleY(1.0f), Offset(0,0){}
	Zoom(float xx, float yy) : ScaleX(xx), ScaleY(yy), Offset(0,0) {}
	PointF	GetPoint(PointF pt) const
	{ 
		return PointF(( pt.X*ScaleX), ( pt.Y*ScaleY)) /*+ Offset*/; 
	}
	PointF	GetInvPoint(PointF pt) const
	{ 
		return PointF(( pt.X/ScaleX), ( pt.Y/ScaleY))/*- Offset*/; 
	}
	SizeF	GetSize(SizeF sz) const
	{ 
		return SizeF((sz.Width*ScaleX), (sz.Height*ScaleY));
	}
	SizeF	GetInvSize(SizeF sz) const
	{ 
		return SizeF((sz.Width/ScaleX), (sz.Height/ScaleY));
	}
	CRectF	GetRect(CRectF rc) const
	{ 
		return CRectF ( rc.X*ScaleX, rc.Y*ScaleY,  rc.Width*ScaleX, rc.Height*ScaleY );
	}
};
#endif // !defined(AFX_ZOOM_H__D2705A11_40F9_48BC_AA61_F989DDEEEDEA__INCLUDED_)
