
#ifndef _WIN_CANVAS_H_included_
#define _WIN_CANVAS_H_included_

#include "my_canvas.h"


class CWinCanvas :public CMyCanvas
{
	const HDC dc;
public:
	CWinCanvas (HDC _dc) :dc(_dc) {}

	HDC get_dc() const { return dc;}

	virtual void line (KPOINT p1, KPOINT p2)
	{
		move_to (p1);		line_to (p2);
	}
	virtual void line_to (KPOINT p)
	{
		::LineTo( dc, p.x, p.y);
	}
	virtual void move_to (KPOINT p)
	{
		::MoveToEx( dc, p.x, p.y, 0);
	}

	virtual void text (KPOINT pos, const _TCHAR * t)
	{
		::TextOut ( dc, pos.x, pos.y, t, _tcslen(t));
	}
	virtual void set_color(long color)
	{
		///
	}

	virtual void cross_point ( KPOINT p, int size)
	{
		int d = size/2;
		KPOINT p0 (p.x + d, p.y + d);
		KPOINT p1 (p.x - d, p.y + d);
		KPOINT p2 (p.x + d, p.y - d);
		KPOINT p3 (p.x - d, p.y - d);

		line( p0, p3);	line( p1, p2);
	}
	
	virtual void circle_point ( KPOINT p, int size)
	{
		int d = size/2;
		::Ellipse( dc, p.x - d , p.y - d, p.x + d, p.y + d );
	}
};

#endif