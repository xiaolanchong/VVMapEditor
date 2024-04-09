#include "stdafx.h"
#include "triangulation.h"

namespace triangulation
{


	/// KCircle ////////////////////////////////////////////
	void KCircle::calc_circle(const CTriang& t)
	{
		KPOINT p1 = t.p[0];	KPOINT p2 = t.p[1]; KPOINT p3 = t.p[2];

		KLine L1 (p1, p2);
		KLine L2 (p1, p3);

		KPOINT p12 ( (p1.x + p2.x), (p1.y + p2.y));
		KPOINT p13 ( (p1.x + p3.x), (p1.y + p3.y));

		KLine Lp1 = L1.perpendikular(p12);
		KLine Lp2 = L2.perpendikular(p13);

		center = Lp1.intersecion(Lp2);


		i64 dx1 = p1.x - p2.x;	i64 dy1 = p1.y - p2.y;
		i64 dx2 = p3.x - p2.x;	i64 dy2 = p3.y - p2.y;
		i64 dx3 = p3.x - p1.x;	i64 dy3 = p3.y - p1.y;

		i64 r1 = (sq(dx1) + sq(dy1));
		i64 r2 = (sq(dx2) + sq(dy2));
		i64 r3 = (sq(dx3) + sq(dy3));

		i64 d = dx1*dy2 - dx2*dy1;

		KFraction RR ( r1*r2*r3, 4*d*d);
		sqR = RR;

		R = (sqrt(RR.float_res()));

	}




	bool KCircle::is_inside(KPOINT p) const 
	{
		i64 dx = p.x - center.x;	i64 dy = p.y - center.y;

		KFraction d2(sq(dx) + sq(dy));
		return (d2 <= sqR );

	}

	KPOINT KCircle::get_p(int angle, int max_angle) const
	{
		double a = 2*pi*double(angle) / double(max_angle);
		long x = (long) (center.x + R*cos(a));
		long y = (long) (center.y + R*sin(a));
		return KPOINT ( x, y);
	}

	/// Triangulation /////////////////////////////////////////////////

	unsigned Triangulation::find_lower_point() const
	{
		long max_y = -10000;
		unsigned res = 0;

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			KPOINT p = m_points[i];
			long the_y = p.y;
			if (the_y > max_y) 
			{ 
				res = i; 	max_y = the_y;
			}

		}
		return res;
	}

	void Triangulation::find_first_edge()
	{

		if (m_points.size() < 2) 
			throw not_enough_points();

		const i64 a_big_num = 1000000000;

		size_t i0 = find_lower_point(), i1 = i0;

		const KPOINT& p0 = m_points[i0];

		KFraction sin2_min( a_big_num );
		i64 min_d2 = a_big_num;

		for (size_t i = 0; i < m_points.size(); ++i)
			//while (it.)
		{
			if (i == i0) continue;

			KPOINT p = m_points[i];

			i64 dx = p.x - p0.x; 
			i64 dy = p.y - p0.y;
			i64 d2 = dx*dx + dy*dy;

			KFraction sin2( dy*dy, d2);
			bool eq = ( (sin2_min == sin2) && (min_d2 > d2) );
			if ( (sin2_min > sin2) || eq)
			{ 
				sin2_min = sin2; min_d2 = d2; i1 = i;
			}
		}
		first_edge = KEdge( i0, i1);

		//ставим знак грани
		int e_sign = 0;
		KLine L( CEdge(first_edge.get_e( m_points )));

		for (size_t j = 0; j < m_points.size(); ++j)
		{
			if (first_edge.is_a_part(j)) continue;
			e_sign = L.side( m_points[j]);
			if (e_sign)	break;
		}
		m_edges.push_back( KEdgeEx( first_edge , e_sign));
	}

	void Triangulation::clear() 
	{	
		first_edge = KEdge();
		m_edges.clear();
		m_triangs.clear();
	};

	bool Triangulation::find_and_erase(KEdgeEx edgex)
	{
		for (edges_list_t::iterator li = m_edges.begin(); li != m_edges.end(); ++li)
		{
			if ((*li) == edgex) { m_edges.erase(li); return true; }
		}
		return false;
	}

	void Triangulation::register_edge(KEdgeEx edgex)
	{
		if (is_on_border(edgex.edge))	return;
		if (find_and_erase(edgex))		return;
		m_edges.push_back(edgex);
	}

	void Triangulation::build_tr(unsigned i, KEdgeEx edgex)
	{
		unsigned i1 = edgex.edge.index[0];
		unsigned i2 = edgex.edge.index[1];

		find_and_erase(edgex);

		//size_t es = edges.size();
		//size_t ts = triangs.size();

		KEdge e1( i, i1);
		KLine L1( e1.get_e(m_points));
		KEdgeEx ex1( e1, -L1.side(m_points[i2]) );
		register_edge( ex1);

		KEdge e2( i, i2);
		KLine L2( e2.get_e(m_points));
		KEdgeEx ex2( e2, -L2.side(m_points[i1]) );
		register_edge( ex2);

		m_triangs.push_back(KTriang( i, i1, i2));
	}

	bool Triangulation::build_one_triangle(KEdgeEx edgex)
	{
		KLine L( edgex.edge.get_e(m_points));
#ifdef _WIN32_WCE
		KFraction min_scos2( 1000000000000000 );
#else
		KFraction min_scos2( 1000000000000000LL );
#endif
		bool point_found = false;
		unsigned found_i = 0;

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			if (edgex.edge.is_a_part(i)) continue;

			KPOINT p = m_points[i];
			if (L.side(p) != edgex.sign) continue;

			KFraction the_scos2 = get_signed_cos2(p, edgex.edge);

			//if (min_scos2.float_res() > the_scos2.float_res())
			if (min_scos2 > the_scos2)
			{
				found_i = i; min_scos2 = the_scos2; point_found = true;
			}
		}
		//строим треугольник
		if (point_found) { build_tr( found_i, edgex); return true; }
		return false;
	}

	bool Triangulation::one_step()
	{
		if (m_edges.empty()) return false;
		return build_one_triangle( m_edges.front());
	}

	void Triangulation::build_all(const point_vect_t& points)
	{
		clear();
		m_points = points;
		find_first_edge();

		if (m_points.size() < 3 ) throw not_enough_points();
		while (one_step());
	}

	bool Triangulation::is_on_border(KEdge edge) const
	{
		KLine L(edge.get_e(m_points));
		int side1 = 0;		int side2 = 0;

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			if ( edge.is_a_part(i) ) continue;
			int s = L.side( m_points[i] );
			if (s < 0) side1++;
			else if (s > 0) side2++;

			if (side1 && side2) return false;
		}
		return true;
	}
	/*
	int Triangulation::L_side(KPOINT p) const
	{
	KLine L(first_edge.get_e(points));
	return int(L.side(p));
	}
	*/
	KFraction Triangulation::get_signed_cos2(KPOINT p, KEdge edge) const
	{
		KPOINT p1 = m_points [edge.index[0]];
		KPOINT p2 = m_points [edge.index[1]];
		i64 dx1 = p1.x - p.x;	i64 dy1 = p1.y - p.y;
		i64 dx2 = p2.x - p.x;	i64 dy2 = p2.y - p.y;
		i64 num = dx1*dx2 + dy1*dy2;
		i64 r12 = (dx1*dx1 + dy1*dy1);
		i64 r22 = (dx2*dx2 + dy2*dy2);
		i64 sign = (num>0) ? (+1) : (-1);

		return KFraction( num*num*sign,  r12*r22);
	}

	CEdge Triangulation::get_first_edge() const 
	{ 
		return first_edge.get_e( m_points);
	}

	bool Triangulation::GetTriang(KPOINT p, CTriang * t) const
	{
		for (triang_vect_t::const_iterator i = m_triangs.begin(); i != m_triangs.end(); ++i)
		{
			CTriang ce = i->get_tr( m_points);
			if (ce.is_inside(p)) { *t = ce; return true;}
		}
		return false;
	}

	bool Triangulation::GetMatrForTriang(KPOINT p, Matr3x2 * m) const
	{
		for (triang_vect_t::const_iterator i = m_triangs.begin(); i != m_triangs.end(); ++i)
		{
			CTriang ce = i->get_tr ( m_points );
			if (ce.is_inside(p)) { *m = i->trans_matr; return true;}
		}
		return false;
	}

	int Triangulation::TriangNum(KPOINT p) const
	{
		int n=0;
		for (triang_vect_t::const_iterator i = m_triangs.begin(); i != m_triangs.end(); ++i, ++n)
		{
			CTriang ce = i->get_tr ( m_points );
			if (ce.is_inside(p)) 
				return n;
		}
		return -1;
	}


	bool Triangulation::test_tr(KTriang t) const
	{
		CTriang ct = t.get_tr ( m_points );
		KCircle c(ct);

		for (size_t i = 0; i < m_points.size(); ++i)
		{
			if (t.contains_i(i)) continue;
			if ( c.is_inside(m_points[i]) ) return false;
		}
		return true;

	}

	bool Triangulation::test() const
	{
		for (triang_vect_t::const_iterator i = m_triangs.begin(); i != m_triangs.end(); ++i)
			if (!test_tr(*i)) return false;

		return true;
	}

	

}