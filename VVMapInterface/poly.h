
#ifndef _POLY_H_included
#define _POLY_H_included

#include "my_canvas.h"
#include "kpoint.h"

/*
class CPoly
{
	typedef std::vector <KPOINT> points_vect;
	typedef points_vect::iterator iter;
	typedef points_vect::const_iterator iterc;
	points_vect points;
public:
	CPoly() {}
	CPoly(RECT r) { init_as_rect(r); }
	void push(KPOINT new_point);
	void push_back(KPOINT new_point) { push(new_point); }
	
	void clear();
	void init(const KPOINT * array, int points_count);
	
	const KPOINT * data() const;
	size_t points_count() const;

	bool is_inside(KPOINT p) const;
	bool is_inside(long x, long y) const { return is_inside(KPOINT (x,y));}

	void init_as_rect( RECT rect);

	void cut_x(int x, bool right);
	void cut_y(int y, bool down);
	void cut_rect(RECT r);
	void draw(CMyCanvas * canvas) const;

	void Move (int dx, int dy);
	KPOINT MassCenterByPerimeter() const;

	int Surface() const;
	int IntersectSurf(const CPoly& other) const;

	
	void save(const char * file) const;
	void load(const char * file);

	bool operator == (const CPoly& p) const;
};
*/
struct CPoly
{
	typedef KPOINT point_type;
	typedef std::vector<point_type> point_vect_t;

	void clear()									{ m_points.clear(); }

	void add(point_type p)							{ push_back(p); }
	void push(point_type p)							{ push_back(p); }
	void push_back(point_type p)					{ m_points.push_back(p); }

	void assign(const point_type * p, int count)	{ m_points.assign ( p, p + count ); }
	void append(const point_type * p, int count)	{ m_points.insert ( m_points.end(), p, p + count ); }
	
	void init_as_rect( RECT rect)	{ *this = CPoly(rect);  }
	void init(const point_type * array, int count)	{ assign ( array, count ); }
		
	size_t count() const							{ return m_points.size(); }
	const point_type * data() const					{ return GetPtr(m_points); }

	size_t points_count() const						{ return count(); }
	
	bool Inside(PointF p) const;
	bool is_inside(KPOINT p) const					{ return Inside(p); }
	bool is_inside( long x, long y) const			{ return Inside(PointF(MyFloat(x),MyFloat(y))); }

	void Move (int dx, int dy);
	void Simplify ();

	point_type MassCenterByPerimeter() const;
	point_type MassCenterBySurface() const;

	int Surface() const;
	MyFloat IntersectSurf(const RectF& rect) const;

	CPoly(){}
	CPoly(RECT r);

	virtual void ser (ser::CElement& e);
	void build_convex();

	void cut_rect(RECT r);
	void cut_y(MyFloat y, bool down, pointf_vect& temp);
	void cut_x(MyFloat x, bool right, pointf_vect& temp);

	bool operator == (const CPoly& p) const;

	void save(const char * file) const;
	void load(const char * file);
private:
	point_vect_t m_points;
};

template <class T>
MyFloat CalcSurface(const std::vector<T>& P)
{
	MyFloat S = 0;
	if (P.empty()) return 0;

	T p0 = P.back();
	for ( std::vector<T>::const_iterator i = P.begin(); i != P.end(); ++i)
	{
		S += (i->x - p0.x)*(i->y + p0.y);
		p0 = *i;
	}
	return abs (S/2);
}


#endif