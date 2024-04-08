// DrawHelper.h: interface for the DrawHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWHELPER_H__5CC525FE_21B9_4907_9D10_382A9BD02841__INCLUDED_)
#define AFX_DRAWHELPER_H__5CC525FE_21B9_4907_9D10_382A9BD02841__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DrawHelper  
{
public:
	DrawHelper();
	virtual ~DrawHelper();

	static void	DrawPointGroup(Graphics& gr, PointF point, int Number, Brush& br);
	static void	DrawPointNumber(Graphics& gr, PointF point, int Number, Brush& br);
};

#endif // !defined(AFX_DRAWHELPER_H__5CC525FE_21B9_4907_9D10_382A9BD02841__INCLUDED_)
