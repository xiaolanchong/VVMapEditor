

#ifndef _TRANS_MATR_H_included_
#define _TRANS_MATR_H_included_

#include "kpoint.h"
#include "fractions.h"
#include "serializer3.h"
#include "arithmetic.h"


	struct Matr3x3
	{
		double values[3][3];
		double det() const
		{
			double t1 = values[0][0]*(values[1][1]*values[2][2] - values[1][2]*values[2][1]);
			double t2 = values[0][1]*(values[1][0]*values[2][2] - values[2][0]*values[1][2]);
			double t3 = values[0][2]*(values[1][0]*values[2][1] - values[2][0]*values[1][1]);
			return t1 - t2 + t3;
		}
	};

	struct Matr3x2
	{
		Matr3x2() :ready(false) {}
		double values[2][3];
		bool ready;

		KPOINT mult (const KPOINT& p)
		{
			double x = values[0][0]*p.x + values[0][1]*p.y + values[0][2];
			double y = values[1][0]*p.x + values[1][1]*p.y + values[1][2];
			return KPOINT ( long(x), long(y) );
		}

		void calc( KPOINT f[3], KPOINT t[3]) 
		{
		
			Matr3x3 m0, m1;

			ready = false;
			int i;
			for ( i=0; i < 3; i++)
			{
				m0.values[i][0] = f[i].x;	m0.values[i][1] = f[i].y;	m0.values[i][2] = 1;
			}

			double det0 = m0.det();
			if (!det0) return;
	//----------------------
			m1 = m0;
			for (i=0; i<3; i++) 
					m1.values[i][0] = t[i].x;
			double det_x0 = m1.det();
	//----------------------
			m1 = m0;
			for (i=0; i <3; i++) 
					m1.values[i][1] = t[i].x;
			double det_x1 = m1.det();
	//----------------------
			m1 = m0;
			for (i=0; i<3; i++)
					m1.values[i][2] = t[i].x;
			double det_x2 = m1.det();
	//----------------------

			m1 = m0;
			for (i=0; i<3; i++)
					m1.values[i][0] = t[i].y;
			double det_y0 = m1.det();
	//----------------------
			m1 = m0;
			for (i=0; i<3; i++)
					m1.values[i][1] = t[i].y;
			double det_y1 = m1.det();
	//----------------------
			m1 = m0;
			for (i=0; i<3; i++)
					m1.values[i][2] = t[i].y;
			double det_y2 = m1.det();
	//----------------------

			values[0][0] = (double(det_x0) / det0);	
			values[0][1] = (double(det_x1) / det0);
			values[0][2] = (double(det_x2) / det0);

			values[1][0] = (double(det_y0) / det0);	
			values[1][1] = (double(det_y1) / det0);
			values[1][2] = (double(det_y2) / det0);

			ready = true;
			return;

		}

		void ser (ser::CElement& e)
		{
			e.attr( _T("triang_ready"), ready);
			e.attr( _T("value_0_0"), values[0][0]);
			e.attr( _T("value_0_1"), values[0][1]);
			e.attr( _T("value_0_2"), values[0][2]);

			e.attr( _T("value_1_0"), values[1][0]);
			e.attr( _T("value_1_1"), values[1][1]);
			e.attr( _T("value_1_2"), values[1][2]);
		}
	};

#endif