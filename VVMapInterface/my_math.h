#ifndef _MY_MATH_H_included_
#define _MY_MATH_H_included_

#include "kpoint.h"
/*
template <class T>
	struct Point2
	{
		T values[2];
		
		Point2() {values[0] = 0; values[1] = 0;}
		Point2(T x, T y) {values[0] = 0; values[1] = 0;}
		Point2(KPOINT p) { values[0] = p.x; values[1] = p.y;}

		T x() const { return values[0];}
		T y() const { return values[1];}
	};

template <class T>
	struct Point3
	{
		T values[3];
	};

template <class T>
	struct Matr3x3
	{
		T values[3][3];
		T det() const
		{
			T t1 = value[0][0]*(value[1][1]*value[2][2] - value[1][2]*value[2][1]);
			T t2 = value[0][1]*(value[1][0]*value[2][2] - value[2][0]*value[1][2]);
			T t3 = value[0][2]*(value[1][0]*value[2][1] - value[2][0]*value[1][1]);
			return t1 - t2 + t3;
		}

/*		
		Matr3x3 (Point3<T> p1, Point3<T> p2, Point3<T> p3)
		{
			for (int i1=0; i1<3; i1++)
				values[0][i1] = p1.values[i1];

			for (int i2=0; i2<3; i2++)
				values[1][i2] = p2.values[i2];

			for (int i3=0; i3<3; i3++)
				values[2][i3] = p3.values[i3];
		}

*/	/*	Point3 operator* (const Point3<T>& p) const 
		{
			Point3<T> res;
			res[0] = values[0][0]*p.values[0] + values[0][1]*p.values[0] + values[0][2];
			res[1] = values[1][0]*p.values[1] + values[1][1]*p.values[1] + values[1][2];
			res[2] = values[2][0]*p.values[2] + values[2][1]*p.values[2] + values[2][2];
		}
	};

template <class T>
	struct Matr3x2
	{
		T values[2][3];

		Point2<T> mult (const Point2<T>& p)
		{
			T x = values[0][0]*p.x() + values[0][1]*p.y() + values[0][2];
			T y = values[1][0]*p.x() + values[1][1]*p.y() + values[1][2];
			return Point2<T> ( x, y);
		}

		//Point3 operator* (const Point3<T>& p) const
	};

*/

#endif