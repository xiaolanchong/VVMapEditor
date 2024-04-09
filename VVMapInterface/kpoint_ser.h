
#ifndef _KPOINT_SER_H_included
#define _KPOINT_SER_H_included

#pragma warning (disable : 4786)

#include <windows.h>
#include "../serializer/serializer3.h"

#include "kpoint.h"


struct KPOINT_ser : KPOINT
{
	KPOINT_ser (long _x, long _y) : KPOINT( _x, _y) {}
	KPOINT_ser (POINT p) : KPOINT( p) {}
	KPOINT_ser ()	{}


	void ser (ser::CElement& e)
	{
		e.attr(_T("x"), x);
		e.attr(_T("y"), y);
	}



	struct KRECT_ser : KRECT
	{
		KRECT_ser (int _left, int _top, int _right, int _bottom) 	
			:KRECT( _left, _top, _right, _bottom) {}

		KRECT_ser (long w, long h) :KRECT ( w, h) {}
		KRECT_ser (KPOINT p1, KPOINT p2):KRECT (p1, p2) {}

		KRECT_ser () {}
		KRECT_ser (RECT r) 	: KRECT (r) {}

		void ser (ser::CElement& e)
		{
			e.attr( _T("left"),	left);
			e.attr( _T("top"),	top);
			e.attr( _T("right"), right);
			e.attr( _T("bottom"), bottom);
		}
	};
};

#endif