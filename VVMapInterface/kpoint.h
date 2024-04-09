
#ifndef _KPOINT_H_included
#define _KPOINT_H_included

#include <windows.h>
#include <math.h>

#ifndef NO_SERIALIZATION
#include "serializer3.h"
#endif

/// класс точки со всеми наворотами
struct KPOINT : POINT
{
	typedef long coord_type;

	KPOINT (long _x, long _y) { x = _x; y = _y; }
	KPOINT (POINT p) { x = p.x; y = p.y; }
	KPOINT () { x = 0; y = 0; }
	//KPOINT (long xx) { x = xx; y = xx; }

	bool not_negative() const { return (x>=0) && (y>=0);}
	inline KPOINT operator+ (const KPOINT& p) const { return KPOINT (x + p.x, y + p.y);}
	inline KPOINT operator- (const KPOINT& p) const { return KPOINT (x - p.x, y - p.y);}
	inline KPOINT& operator+= (const KPOINT& p) { x += p.x; y += p.y; return *this;}
	inline KPOINT& operator-= (const KPOINT& p) { x -= p.x; y -= p.y; return *this;}
	inline bool operator== (const KPOINT& p) const { return (x == p.x) && (y == p.y);}
	inline bool operator!= (const KPOINT& p) const  { return !(operator==(p));}
	inline KPOINT operator* (MyFloat a) const { return KPOINT (long (a*x + 0.5), long(a*y + 0.5));}
	inline KPOINT operator* (long a) const { return KPOINT (a*x, a*y);}
	inline long operator* (const KPOINT& p) const { return x*p.x +y*p.y;}

	double distance(const KPOINT& p) const { return sqrt ( (double)sqr_distance(p)); } 

	long sqr_distance(const KPOINT& p) const
	{
		long dx = (p.x - x), dy = (p.y - y);
		return dx*dx + dy*dy;
	}

	bool is_near(const KPOINT& p, long delta = 1) const 
	{ 
		long dx = abs(x - p.x);
		long dy = abs(x - p.y);
		return (dx <= delta) && (dy <= delta);
	}
#ifndef NO_SERIALIZATION
	void ser (ser::CElement& e)
	{
		e.attr(_T("x"), x);
		e.attr(_T("y"), y);
	}
#endif

	bool valid() const { return (x>=0) && (y>=0);}
	bool is_null() const { return !x && !y;}
};

inline KPOINT make_point (long x=0, long y=0) { return KPOINT(x,y); }
/*
struct DPOINT
{
	double x; 
	double y;
	DPOINT (double _x, double _y) :x(_x), y(_y) {}
	DPOINT () :x (0), y(0) {}

	bool not_negative() const { return (x>=0) && (y>=0);}

	inline DPOINT  operator+ (const DPOINT& p) const { return DPOINT (x + p.x, y + p.y);}
	inline DPOINT  operator- (const DPOINT& p) const { return DPOINT (x - p.x, y - p.y);}
	inline DPOINT& operator+= (const DPOINT& p) { x += p.x; y += p.y; return *this;}
	inline DPOINT& operator-= (const DPOINT& p) { x -= p.x; y -= p.y; return *this;}


	bool is_near(const DPOINT& p, long delta = 1) const 
	{ 
		long dx = abs(x - p.x);
		long dy = abs(x - p.y);
		return (dx <= delta) && (dy <= delta);
	}

	void ser (ser::CElement& e)
	{
		e.attr(_T("x"), x);
		e.attr(_T("y"), y);
	}

	double distance(const KPOINT& p) const { sqrt (sqr_distance(p)); } 

	double sqr_distance(const KPOINT& p) const
	{
		double dx = (p.x - x);	double dy = (p.y - y);
		return dx*dx + dy*dy;
	}

};
*/

struct KRECT : RECT
{
	KRECT (int _left, int _top, int _right, int _bottom) 	
	{ left = _left; top = _top; right = _right; bottom = _bottom;}
	KRECT (long w, long h) { left = 0; top = 0; right = w; bottom = h;}
	KRECT (KPOINT p1, KPOINT p2) 
	{ 
		left	= (p1.x < p2.x) ? (p1.x) : (p2.x); 
		top		= (p1.y < p2.y) ? (p1.y) : (p2.y); 
		right	= (p1.x > p2.x) ? (p1.x) : (p2.x);  
		bottom	= (p1.y > p2.y) ? (p1.y) : (p2.y); 
	}
	KRECT () { left = 0; top = 0; right = 0; bottom = 0;}
	KRECT (RECT r) 	
	{ left = r.left; top = r.top; right = r.right; bottom = r.bottom;}

	KPOINT center() const 
	{ 
		long dx = (right - left)/2;		long dy = (bottom - top)/2;
		return KPOINT (left + dx, top + dy);
	}

	bool is_inside (KPOINT p) const 
	{
		long xmin = (left < right) ? (left) : (right);
		long xmax = (left > right) ? (left) : (right);
		long ymin = (top < bottom) ? (top) : (bottom);
		long ymax = (top > bottom) ? (top) : (bottom);

		return (xmin <= p.x) && (p.x < xmax) && (ymin <= p.y) && (p.y < ymax);
	}
#ifndef NO_SERIALIZATION
	void ser (ser::CElement& e)
	{
		e.attr( _T("left"),	left);
		e.attr( _T("top"),	top);
		e.attr( _T("right"), right);
		e.attr( _T("bottom"), bottom);
	}
#endif
};

/////////////////////////////////////////////////////////////////////////////////////////////
/*
inline POINT make_point(long x = 0, long y = 0) { POINT p; p.x=x;p.y=y; return p;}
inline POINT operator + (POINT p1, POINT p2)	{ return make_point(p1.x+p2.x,p1.y+p2.y);}
inline POINT operator - (POINT p1, POINT p2)	{ return make_point(p1.x-p2.x,p1.y-p2.y);}
inline POINT& operator += (POINT& p, POINT p1)	{ p.x+=p1.x; p.y+=p1.y; return p;}
inline POINT& operator -= (POINT& p, POINT p1)	{ p.x-=p1.x; p.y-=p1.y; return p;}

inline long operator * (POINT p1, POINT p2)	{ return p1.x*p2.x + p1.y*p2.y; }

inline POINT operator *(POINT p, MyFloat a) { return make_point( long (p.x*a + 0.5), long (p.y*a + 0.5)); }
inline POINT operator /(POINT p, MyFloat a) { return make_point( long (p.x/a + 0.5), long (p.y/a + 0.5)); }
inline bool operator == (POINT p1, POINT p2)	{ return p1.x==p2.x && p1.y==p2.y; }
inline bool operator != (POINT p1, POINT p2)	{ return !(p1==p2); }
inline MyFloat distance (POINT p1, POINT p2) { return sqrt ( MyFloat ( sqr(p1.x - p2.x) + sqr(p1.y - p2.y) ) ); }
*/

bool Intersect(KPOINT p1, KPOINT p2, KPOINT q1, KPOINT q2, KPOINT& result);

struct PointF
{
	typedef MyFloat coord_type; 
	MyFloat x,y;
	PointF() : x(MyFloat(0.0)), y(MyFloat(0.0)) {}
	PointF(MyFloat xx, MyFloat yy) : x(xx), y(yy) {}
	PointF(KPOINT p) : x(MyFloat(p.x)), y(MyFloat(p.y)) {}
	operator KPOINT() const { return KPOINT( long (x+MyFloat(0.5)),  long (y+MyFloat(0.5)) ); }
};

inline PointF make_pointf(MyFloat x = MyFloat(0.0), MyFloat y = MyFloat(0.0)) {	return PointF(x,y); }

inline PointF operator + (PointF p1, PointF p2)		{ return  PointF( p1.x + p2.x, p1.y + p2.y); }
inline PointF operator - (PointF p1, PointF p2)		{ return  PointF( p1.x - p2.x, p1.y - p2.y); }
inline PointF& operator += (PointF& p, PointF p2)	{ p.x += p2.x; p.y += p2.y; return p; }
inline PointF& operator -= (PointF& p, PointF p2)	{ p.x -= p2.x; p.y -= p2.y; return p; }

inline MyFloat operator * (PointF p1, PointF p2)		{ return p1.x*p2.x + p1.y*p2.y; }

inline PointF operator *(PointF p, MyFloat a)		{ return PointF (p.x*a,p.y*a); }
inline PointF operator /(PointF p, MyFloat a)		{ return PointF (p.x/a,p.y/a); }
inline bool operator == (PointF p1, PointF p2)		{ static const MyFloat eps = MyFloat(1e-10); return abs(p1.x-p2.x)<eps && abs(p1.y-p2.y)<eps; }
inline bool operator != (PointF p1, PointF p2)		{ return !(p1 == p2); }

bool Intersect(PointF p1, PointF p2, PointF q1, PointF q2, PointF& result);


inline MyFloat distance(PointF p1, PointF p2) {	return sqrt (  sqr(p1.x - p2.x) + sqr(p1.y - p2.y) ); }

struct RectF
{
	MyFloat left, top, right, bottom;
	RectF() : left(0), top(0), right(0), bottom(0) {}
	RectF(MyFloat L, MyFloat T, MyFloat R, MyFloat B) : left(L), top(T), right(R), bottom(B) {}
	RectF(RECT r) : left(MyFloat(r.left)), top(MyFloat(r.top)), right(MyFloat(r.right)), bottom(MyFloat(r.bottom)) {}
	operator RECT() const 
	{ 
		RECT r; 
		r.left		= long (left	+ 1/MyFloat(2));
		r.top		= long (top		+ 1/MyFloat(2));
		r.right		= long (right	+ 1/MyFloat(2));
		r.bottom	= long (bottom	+ 1/MyFloat(2));
		return r;
	}

};

inline bool inside (MyFloat x, MyFloat x1, MyFloat x2) {return std::min (x1, x2) <= x && x <= std::max (x1, x2);}

class Line
{
	// прямая задается направляющим вектором { Lx, Ly } (его длина == 1)
	// и расстоянием этой прямой от начала координат _d
	// уравнение прямой {- Ly*x + Lx*y + D = 0}

	MyFloat Lx, Ly, D;
public:
	// проекция точки на прямую
	MyFloat project(POINT p) const
	{
		return Lx*p.x + Ly*p.y;
	}

	// проекция точки на прямую
	MyFloat project(MyFloat x, MyFloat y) const
	{
		return Lx*x + Ly*y;
	}

	MyFloat project(PointF p) const
	{
		return project ( p.x, p.y );
	}

	// конструктор
	Line(PointF p1, PointF p2)
	{ 
		MyFloat d = distance(p1, p2); 
		Lx = (p1.x - p2.x) / d;
		Ly = (p1.y - p2.y) / d;
		D =  Ly*p1.x - Lx*p1.y;
		MyFloat D1 = Ly*p2.x - Lx*p2.y;
	}

	bool Intersect(const Line& L, MyFloat& x, MyFloat& y)
	{
		// -Ly * x + Lx * y = - D
		MyFloat det = - Ly * L.Lx + Lx * L.Ly;
		if (abs(det) < 1e-10) return false;

		MyFloat det1 =  (-D) * L.Lx - Lx * (- L.D);
		MyFloat det2 = - Ly * (- L.D) + ( -D)* L.Ly;

		x = det1 / det;	y = det2 / det;

		assert ( abs (- Ly * x + Lx * y + D) < 1e-6);
		assert ( abs (- L.Ly * x + L.Lx * y + L.D) < 1e-6);

		return true;
	}

	bool IsParallel(const Line& L) const
	{
		static const MyFloat eps = MyFloat(1e-5);
		return abs(Ly - L.Ly) < eps && abs(Lx - L.Lx) < eps;
	}
};

typedef std::vector<KPOINT> point_vect_t;
typedef std::vector<KRECT> krect_vect_t;

// шаблон, позволяющий узнать тип координаты для POINT или PointF
template<class P>struct PointTraits{typedef typename P::coord_type coord_type;};
// специализация PointTraits для POINT
template<> struct PointTraits<POINT>{typedef long coord_type;};

// функтор, получающий координату x для точки. 
// использование: getX<POINT> X; POINT P; X(P) = 5; std::cout << X(P);
template<class P> struct getX
{
	typedef P point_type; 
	typedef typename PointTraits<P>::coord_type coord_type;
	coord_type& operator()(P& p) const { return p.x; }
	const coord_type& operator()(const P& p) const { return p.x; }
};

// аналогично getX для Y
template<class P> struct getY
{
	typedef P point_type;
	typedef typename PointTraits<P>::coord_type coord_type;
	coord_type& operator()(P& p) const { return p.y; }
	const coord_type& operator()(const P& p) const { return p.y; }
};

/// определяет type как getX для getY и как getY для getX
template <class G> struct getOther;
template <class P> struct getOther< getX<P> > {	typedef getY<typename getX<P>::point_type> type;};
template <class P> struct getOther< getY<P> > {	typedef getX<typename getY<P>::point_type> type;};

typedef std::vector<PointF> pointf_vect; 
typedef std::vector<KPOINT> points_vect;

/////////
template <class Res, class Src>
Res MyRound(Src x) { return Res(x); }

template <>
inline long MyRound<long,MyFloat>(MyFloat x) { return long(x+1/MyFloat(2)); }



#endif