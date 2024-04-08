// FigPolyline.h: interface for the CFigPolyline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIGPOLYLINE_H__404A7D7C_307B_4328_B374_4D355B3106E7__INCLUDED_)
#define AFX_FIGPOLYLINE_H__404A7D7C_307B_4328_B374_4D355B3106E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct Zoom;

class CFigPolyline  
{
	std::vector<PointF>	m_Points;
	PointF				m_EndPoint;
	bool				m_IsEndPointValid;
public:
	CFigPolyline(PointF Start);
//	CFigPolyline(const std::vector<PointF>& Points);
	
	virtual ~CFigPolyline();

	virtual void Draw(Graphics& gr, const Zoom& zoom, bool bCurrent) const ;
	virtual void DrawEdit(CWnd* pWnd, const Zoom& zoom) const ;
	virtual void InvalidateEdit(CWnd* pWnd, const Zoom& zoom) const ;

/*	virtual void AddPoint(PointF Point);
	virtual void SetEndPoint(PointF point);
	virtual void OffsetEndPoint(SizeF size);
			void	EndEdit(PointF point);*/

	void		LButtonDown	( PointF point );
	void		LButtonUp	( PointF point );
	void		LButtonDbl	( PointF point );
	void		MouseMove	( PointF point );
	bool		NeedRedrawWhenMousMove() const { return m_IsEndPointValid;}

	static	void	Draw(	Graphics& gr, 
							const Zoom& zoom, 
							const std::vector<PointF>& Points) ;
	const std::vector<PointF>&	GetPoints() const { return m_Points;}
};

#endif // !defined(AFX_FIGPOLYLINE_H__404A7D7C_307B_4328_B374_4D355B3106E7__INCLUDED_)
