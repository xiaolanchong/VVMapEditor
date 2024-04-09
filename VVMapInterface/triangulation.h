#ifndef _VVMInterface_triangulation_h_included_
#define _VVMInterface_triangulation_h_included_

#include "fractions.h"
#include "points.h"
#include "trans_matr.h"

namespace triangulation
{

	//грань
	struct CEdge
	{
		CEdge(KPOINT p1, KPOINT p2) { p[0] = p1; p[1] = p2; }
		KPOINT p[2];
	};

	struct KEdge
	{
		KEdge() {index[0] = 0; index[1] = 0; }
		KEdge(unsigned i1, unsigned i2) {index[0] = i1; index[1] = i2; }

		CEdge get_e(const point_vect_t& points) const
		{ 
			return CEdge( points [index[0]], points [index[1]] );
		}

		unsigned index[2];
		bool operator== (const KEdge& edge) const
		{
			bool c1 = (index[0] == edge.index[0]) && (index[1] == edge.index[1]);
			bool c2 = (index[1] == edge.index[0]) && (index[0] == edge.index[1]);
			return (c1 || c2);
		}

		bool is_a_part(unsigned the_index) const { return (the_index == index[0]) && (the_index == index[1]);}
	};

	struct KEdgeEx 
	{
		KEdge edge;
		int sign;
		KEdgeEx() : sign(0) {}
		KEdgeEx(KEdge e, int s) : edge(e), sign(s) {}
		bool operator== (const KEdgeEx& edgex) const {return (edge == edgex.edge);}
	};
	typedef std::list <KEdgeEx> edges_list_t;


	struct KLine 
	{
		i64 a;	i64 b; i64 c;
		KLine(CEdge e) : 
		a( calc_a( e.p[0], e.p[1]) ),
			b( calc_b( e.p[0], e.p[1]) ),
			c( calc_c( e.p[0], e.p[1]) )
		{}

		KLine(KPOINT p1, KPOINT p2) :
		a( calc_a( p1, p2) ),
			b( calc_b( p1, p2) ),
			c( calc_c( p1, p2) )
		{}

		KLine() : a(0), b(0), c(0) {}

		i64 point_val(KPOINT p) const { return a*p.x + b*p.y + c; }
		int side(KPOINT p) const 
		{
			i64 pv = point_val(p);
			if (pv>0) return +1;
			if (pv<0) return -1;
			return 0;
		}

		KLine perpendikular(KPOINT p) const
		{
			KLine lp;		lp.a = b;	lp.b = -a; lp.c = 0;
			lp.c = - lp.point_val(p);
			lp.a *= 2;	lp.b *= 2;
			return lp;
		}

		KPOINT intersecion(KLine L) const
		{
			KLine L1, L2;
			if (L.a != 0) { L1 = L; L2 = *this;}
			else { L1 = *this; L2 = L; }

			i64 det0 = L1.a*L2.b - L2.a*L1.b;
			i64 detx = (-L1.c)*L2.b - (-L2.c)*L1.b;
			i64 dety = L1.a*(-L2.c) - L2.a*(-L1.c);

			KFraction fx( detx , det0);
			KFraction fy( dety , det0);

			return KPOINT( long(fx.float_res()+0.5), long(fy.float_res()+0.5));

		}

	private:
		i64 calc_a( KPOINT p1, KPOINT p2) const	{ return p2.y - p1.y;}
		i64 calc_b( KPOINT p1, KPOINT p2) const	{ return p1.x - p2.x;}
		i64 calc_c( KPOINT p1, KPOINT p2) const	{ return p1.x*(p1.y - p2.y) + p1.y*(p2.x - p1.x);}
	};

	struct CTriang
	{
		CTriang() { p[0] = KPOINT(); p[1] = KPOINT(); p[2] = KPOINT(); }
		CTriang(KPOINT p1, KPOINT p2, KPOINT p3) { p[0] = p1; p[1] = p2; p[2] = p3; }
		KPOINT p[3];

		bool is_inside (KPOINT pp) const
		{
			KLine L1(p[0], p[1]);		
			KLine L2(p[1], p[2]);
			KLine L3(p[2], p[0]);

			bool c1 = ( (L1.side(pp) == L1.side(p[2])) || !L1.side(pp) );
			bool c2 = ( (L2.side(pp) == L2.side(p[0])) || !L2.side(pp) );
			bool c3 = ( (L3.side(pp) == L3.side(p[1])) || !L3.side(pp) );

			return c1 && c2 && c3;
		}
	};



	//треугольник
	struct KTriang
	{
		KTriang() { index[0] = 0; index[1] = 0; index[2] = 0;}
		KTriang( unsigned i1, unsigned i2, unsigned i3) 
		{ 
			index[0] = i1; index[1] = i2; index[2] = i3;
		}

		long index[3];

		Matr3x2 trans_matr;

		bool contains_i(unsigned i) const 
		{ 
			return (unsigned(index[0]) == i) 
				|| (unsigned(index[1]) == i) 
				|| (unsigned(index[2]) == i);
		}
		CTriang get_tr(const point_vect_t& points) const
		{
			return CTriang(	points[index[0]], points[index[1]], points[index[2]] );
		}

		void ser(ser::CElement& e)
		{
			e.attr(_T("Point_index_0"), index[0]);
			e.attr(_T("Point_index_1"), index[1]);
			e.attr(_T("Point_index_2"), index[2]);
			e.ser_obj ( _T("matrix"), trans_matr );
		}
	};
	typedef std::vector <KTriang> triang_vect_t;


	struct KCircle 
	{
		KPOINT center;
		double R;
		KFraction sqR;
		const double pi;
		KCircle () : R(0), pi ( 3.141592653589793238462643f ) {}
		KCircle (KPOINT _center, double _R) : center(_center), R(_R), pi ( 3.141592653589793238462643f ) {}

		KCircle (CTriang tr) : pi ( 3.141592653589793238462643f ) {calc_circle(tr);}


		bool is_inside(KPOINT p) const; 
	private:
		void calc_circle(const CTriang& t);
		KPOINT get_p(int angle, int max_angle) const;
	};

	class not_enough_points {};

	struct Triangulation
	{
		Triangulation() {}
		void build_all(const point_vect_t& points);
		bool test() const;
		void clear();
		triang_vect_t& triang_vect() { return m_triangs; }
		bool GetTriang(KPOINT p, CTriang * t) const;
		bool GetMatrForTriang(KPOINT p, Matr3x2 * m) const;
		bool test_tr(KTriang t) const;
		int TriangNum(KPOINT p) const;
		void SetPointsArray(const point_vect_t& points) { m_points = points; }
	private:
				
		point_vect_t m_points;
		edges_list_t m_edges;
		triang_vect_t m_triangs;
		KEdge first_edge;

		//------------------------------------------------------------------
		CEdge get_first_edge() const;
		unsigned find_lower_point() const;
		bool find_and_erase(KEdgeEx edgex);
		void register_edge(KEdgeEx edgex);
		bool is_on_border(KEdge edge) const;
		KFraction get_signed_cos2(KPOINT p, KEdge edge) const;
		void build_tr(unsigned i, KEdgeEx edgex);
		bool build_one_triangle(KEdgeEx edgex);
		void find_first_edge();		
		bool one_step();
	};

	template <class TPoint>
	struct cmp_y{bool operator()(TPoint p1, TPoint p2) {return p1.y < p2.y;}};

	template <class TPoint>
	std::vector<TPoint> GetConvexShell(const std::vector<TPoint>& points)
	{
		typedef TPoint KPOINT;
		typedef std::vector<TPoint> point_vect_t;
		typedef point_vect_t::const_iterator iterc;
		
		point_vect_t result;
		result.reserve (points.size());

		if (points.size() <= 3)	return points;

		iterc start_point = std::max_element( ALL(points), cmp_y<TPoint>());
		iterc first_point = start_point;
		iterc second_point = points.end();
		iterc third_point = points.end();

		result.push_back (*first_point);

		double max_cos = 0;
		iterc i;
		for (i = points.begin(); i != points.end(); ++i)
		{
			if (i == first_point) continue;
			double _cos = (i->x - first_point->x) / first_point->distance ( *i );
			if ( abs(_cos) > max_cos) 
			{ 
				max_cos = abs(_cos); second_point = i; 
			}
		}
		result.push_back ( *second_point);

		for(;;)
		{
			//if ((i == first_point) || (i == second_point)) continue;
			KPOINT d1 = *first_point - *second_point;
			double _d1 = d1.distance( KPOINT());
			double min_cos = 2;
			for (iterc j = points.begin(); j != points.end(); ++j) 
			{
				if ((j == first_point) || (j == second_point)) continue;
				KPOINT d2 = *j - *second_point;
				double _d2 = d2.distance( KPOINT() );
				double _cos = (d1*d2) / (_d1*_d2) ;
				if (_cos < min_cos) 
				{ 
					third_point = j; min_cos = _cos; 
				}
			}

			if (third_point == start_point) break;

			if (third_point != points.end()) 
				result.push_back( *third_point );

			first_point = second_point;
			second_point = third_point;
			third_point = points.end();
		}


		return result;
	}
}

#endif