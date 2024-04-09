
#include "stdafx.h"

#include "poly.h"
#include "fractions.h"


int CPoly::Surface() const
{
	return (int) CalcSurface(m_points);
}

//------------------------------------------------------
void CPoly::Move (int dx, int dy)
{
	for ( points_vect::iterator i = m_points.begin(); i != m_points.end(); ++i)
		i->x += dx,	i->y += dy;
}
//------------------------------------------------------

KPOINT CPoly::MassCenterByPerimeter() const
{
	KPOINT res = make_point();
	KPOINT p0 = m_points.back();
	MyFloat sL = 0;
	for ( points_vect::const_iterator i = m_points.begin(); i != m_points.end(); ++i)
	{
		MyFloat L = sqrt ( MyFloat ( sq(i->x - p0.x) + sq (i->y - p0.y) ) );
		sL += L;
		res += (p0 + *i) * (L/2);
		p0 = *i;
	}
	return sL > 0 ? res*(1/sL) : res;
}

KPOINT CPoly::MassCenterBySurface() const
{
	if (m_points.empty()) return make_point();

	KPOINT p0 = m_points.back();
	MyFloat Sx = 0, Sy = 0;
	MyFloat mx = 0, my = 0;
	for ( points_vect::const_iterator i = m_points.begin(); i != m_points.end(); ++i)
	{
		long dx = i->x - p0.x;
		long dy = i->y - p0.y;
		
		if (dx != 0)
		{
			MyFloat a = MyFloat (dy) / dx;
			MyFloat b =  p0.y - a*p0.x;
			MyFloat s = dx*(i->y + p0.y)/ MyFloat(2);
			mx += (a*(cube(i->x) - cube(p0.x))/3 + b*(sq(i->x) - sq(p0.x))/2);
			Sx += s;
		}

		if (dy != 0)
		{
			MyFloat a = MyFloat (dx) / dy;
			MyFloat b = p0.x - a*p0.y;
			MyFloat s = dy*(i->x + p0.x)/ MyFloat(2);
			my += (a*(cube(i->y) - cube(p0.y))/3 + b*(sq(i->y) - sq(p0.y))/2);
			Sy += s;
		}
		p0 = *i;
	}
	return make_point ( long (mx/Sx + 1/MyFloat(2)), long (my/Sy + 1/MyFloat(2)) );
}

void CPoly::Simplify ()
{
	if (m_points.size() < 3) return;
	points_vect::iterator pi = m_points.begin();
	while (pi != m_points.end())
	{
		KPOINT p1 = *pi;
		KPOINT p0 = ( pi != m_points.begin() ? *(pi-1) : m_points.back());
		KPOINT p2 = ((pi+1) != m_points.end() ? *(pi+1) : m_points.front());

		Line L (p0, p2);
		if ( L.IsParallel (Line ( p0, p1)) 
			 && inside ( L.project (p1), L.project(p0), L.project(p2)) )
		{
			pi = m_points.erase(pi);
		}
		else
			++pi;
	}
}

int gde_oy_peresekaet_otrezok( PointF the_point, POINT p1, POINT p2)
{
	long min_y = std::min(p1.y, p2.y);
	long max_y = std::max(p1.y , p2.y);

	if ( (min_y > the_point.y) || (max_y <= the_point.y) ) return 0;

	long sign = ( (p2.y - p1.y) > 0 ) ? (+1) : (-1);//*(p2.x - p1.x)

	MyFloat L1 = (the_point.x - p1.x) * (p2.y - p1.y);
	MyFloat L2 = (the_point.y - p1.y) * (p2.x - p1.x);
	
	return (L1 < L2) ? (-sign) : (+sign);
}

bool CPoly::Inside(PointF p) const
{
	if (m_points.empty()) return false;

	bool left = false;
	bool right = false;

	POINT t = m_points.back();
	
	points_vect::const_iterator i = m_points.begin();
	for (; i != m_points.end(); ++i)
	{
		int g = gde_oy_peresekaet_otrezok( p, t, *i);
		if (g>0) right = !right;
		if (g<0) left = !left;
		t = *i;
	}

	return (left && right);
}

namespace{struct cmp_y{bool operator()(KPOINT p1, KPOINT p2){return p1.y < p2.y;}};}

void CPoly::build_convex()
{
	if (points_count() < 3) return;
	typedef points_vect::const_iterator iterc;
	iterc start_point = std::max_element( ALL(m_points), cmp_y());
	iterc first_point = start_point;
	iterc second_point = m_points.end();
	iterc third_point = m_points.end();
	points_vect result;
	result.reserve (m_points.size());
	 
	result.push_back (*first_point);

	double max_cos = -2;
	iterc i = m_points.begin();
	for (; i != m_points.end(); ++i)
	{
		if (i == first_point) continue;
		double _cos = (i->x - first_point->x) / first_point->distance(*i);
		if ( _cos > max_cos) { max_cos = _cos; second_point = i; }
	}
	assert (second_point != m_points.end());
	result.push_back (*second_point);

	//for (i = points.begin(); i != points.end(); ++i)
	for(;;)
	{
		//if ((i == first_point) || (i == second_point)) continue;
		KPOINT d1 = *first_point - *second_point;
		double _d1 = d1.distance(KPOINT());
		double min_cos = 2;
		for (iterc j = m_points.begin(); j != m_points.end(); ++j) 
		{
			if ((j == first_point) || (j == second_point)) continue;
			KPOINT d2 = *j - *second_point;
			double _d2 = d2.distance(KPOINT());
			double _cos = (d1*d2) / (_d1*_d2) ;
			if (_cos < min_cos) 
			{ 
				third_point = j; min_cos = _cos; 
			}
		}

		if (third_point == start_point) break;

		if (third_point != m_points.end()) result.push_back(*third_point);
		first_point = second_point;
		second_point = third_point;
		third_point = m_points.end();
	}


	m_points.swap(result);
}

//////////////////////////////////////////////////////////

template<class P>
struct Cut
{
	typedef typename PointTraits<P>::coord_type num_type;
	typedef std::vector<P> p_vect;

	template <class GetX>
	static void cut(p_vect& polygon, num_type x, bool more, GetX X, p_vect * temp_array = 0)
	{
		typename getOther < GetX >::type Y;
		p_vect _temp;
		p_vect& temp = temp_array ? *temp_array : _temp;

		if (polygon.empty()) return;

		//pvect temp; 
		temp.erase (ALL(temp));
		temp.reserve( polygon.size()*2);	
		
		long sign = (more) ? (+1) : (-1);
		P t = polygon.back();

		for (p_vect::const_iterator i = polygon.begin(); i != polygon.end(); ++i)
		{
			num_type the_y;
			if (get_x( &the_y, x, t, *i, Y))
			{
				temp.push_back( make_p( x, the_y, X));
			}
			
			temp.push_back( *i);
			t = *i;
		}

		polygon.erase( ALL(polygon) );
		polygon.reserve(temp.size());

		for (p_vect::const_iterator it = temp.begin(); it != temp.end(); ++it)
			if (X(*it)*sign >= x*sign) 
					polygon.push_back(*it);

	}
private:

	template <class GetX>
	static P make_p(num_type x, num_type y, GetX X)
	{
		typename getOther < GetX >::type Y;
		P p; X(p) = x; Y(p) = y;
		return p;
	}
	
	template <class GetX>
	static bool get_x(num_type * x, num_type y, P p1, P p2, GetX X)
	{
		typename getOther < GetX >::type Y;
		num_type min_y = (Y(p1) < Y(p2)) ? Y(p1) : Y(p2);
		num_type max_y = (Y(p1) > Y(p2)) ? Y(p1) : Y(p2);

		if ( (min_y > y) || (max_y < y) ) return false;

		MyFloat dx = MyFloat(X(p2) - X(p1)), dy = MyFloat(Y(p2) - Y(p1));
		if (abs(dy) < 1e-8) return false;
		*x = MyRound<num_type> (X(p1) + (y - Y(p1))*(dx/dy));
		return true;
	}


};


void CPoly::cut_rect(RECT r)
{
	points_vect temp;
	Cut<KPOINT>::cut( m_points, r.left,		true,	getX<KPOINT>(), &temp);
	Cut<KPOINT>::cut( m_points, r.top,		true,	getY<KPOINT>(), &temp);
	Cut<KPOINT>::cut( m_points, r.right,	false,	getX<KPOINT>(), &temp);
	Cut<KPOINT>::cut( m_points, r.bottom,	false,	getY<KPOINT>(), &temp);
}

MyFloat CPoly::IntersectSurf(const RectF& rect) const
{
#ifndef TRY_INT
	pointf_vect pf (m_points.size()), temp;
	std::copy ( ALL(m_points), pf.begin() );
	typedef PointF P;
	Cut<P>::cut( pf, rect.left,		true, getX<P>(), &temp);
	Cut<P>::cut( pf, rect.top,		true, getY<P>(), &temp);
	Cut<P>::cut( pf, rect.right,	false, getX<P>(), &temp);
	Cut<P>::cut( pf, rect.bottom,	false, getY<P>(), &temp);

	return CalcSurface(pf);
#else	
	CPoly tp = *this;
	tp.cut_rect (rect);
	return tp.Surface();
#endif
	
}
//////////////////////////////////////////////////////////

void CPoly::ser (ser::CElement& e)
{
	e.ser_vect( _T("point"), m_points);
}

void CPoly::save(const char * file) const
{
	std::ofstream os(file);
	for (points_vect::const_iterator i = m_points.begin(); i != m_points.end(); ++i)
		os << i->x << "\t" << i->y << std::endl;
	os.close();
}

void CPoly::load(const char * file)
{
	std::ifstream is(file);
	m_points.clear();
	m_points.reserve (1000);
	std::istream_iterator<long> i(is);
	std::istream_iterator<long> end;

	while (i != end)
	{
		long x = *i++;
		if (i == end) break;
		long y = *i++;
		m_points.push_back( KPOINT(x,y) );
	}
	is.close();
}

