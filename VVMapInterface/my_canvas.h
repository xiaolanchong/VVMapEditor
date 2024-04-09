
#ifndef _MY_CANVAS_H_included_
#define _MY_CANVAS_H_included_

#include "kpoint.h"
#include <tchar.h>


class CMyCanvas
{	
public:
	enum colors
	{
		col_red = 0xff0000,
		col_blue = 0x0000ff,
		col_green = 0x00ef00,
		col_white = 0xffffff,
		col_black = 0x000000,
	};

	virtual void line (KPOINT p1, KPOINT p2) = 0;
	virtual void move_to (KPOINT p) = 0;
	virtual void line_to (KPOINT p) = 0;
	virtual void text (KPOINT pos, const _TCHAR * t) = 0;
	virtual void set_color(long color) = 0;

	virtual void cross_point ( KPOINT p, int size = 6) = 0;
	virtual void circle_point ( KPOINT p, int size = 6) = 0;
	virtual void rect ( KRECT r) 
	{
		line(KPOINT(r.left, r.top),		KPOINT(r.right, r.top));
		line(KPOINT(r.right, r.bottom), KPOINT(r.right, r.top));
		line(KPOINT(r.right, r.bottom), KPOINT(r.left, r.bottom));
		line(KPOINT(r.left, r.top),		KPOINT(r.left, r.bottom));
	}
};

#endif