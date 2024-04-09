#include "stdafx.h"
#include "points.h"



inline bool is_val( long x, long y) { return (x>=0) && (y>=0);}
inline bool is_ch( long x1, long x2) 
{ 
	bool v1 = !(x1<0);		bool v2 = !(x2<0);

	if (!v1 && !v2) return false;
	if ((!v1 && v2) || (v1 && !v2)) return true;
	
	return (x1 != x2) ;
}

inline bool is_ch( const KPOINT& p1, const KPOINT& p2) 
{ 
	bool v1 = p1.valid();		bool v2 = p2.valid();

	if (!v1 && !v2) return false;
	if ((!v1 && v2) || (v1 && !v2)) return true;
	
	return (p1 != p2) ;
}

int CPointDump::correct_master_index ( int master_index) const
{
	if ( (master_index < 0) || (master_index >= (int) master_count) ) return -1;
	else return master_index;
}

int CPointDump::indx ( int master_index, int point_index) const 
{ 
	int mn = (master_index != master_count)	? (master_index) : (-1);
	return  point_index * (master_count+1)  + (mn+1) ;
}

int CPointDump::back_master_indx ( int index) const 
{ 
	return  index%(master_count+1) - 1;
}

int CPointDump::back_point_indx ( int index) const 
{ 
	return  index/(master_count+1);
}

		
CPointDump::CPointDump(): master_count(0), points_count(0) {}


CPointDump::CPointDump(int _master_count, int _points_count) 
	:master_count(_master_count), points_count(_points_count),

	 cont( (_master_count + 1) * _points_count),
	 changes( cont.size())

{
	reset_points();
	all_changed(true);
}


void CPointDump::reset_points()
{
	for ( points_iter it = cont.begin(); it != cont.end(); ++it) 
		*it = KPOINT(-1,-1);
}

void CPointDump::all_changed(bool value)
{
	for ( bool_iter it = changes.begin(); it != changes.end(); ++it) 
		*it = value;
}

void CPointDump::resize()
{
	CPointDump pd(master_count, points_count);

	//копирует точки
	for (int mi = -1; mi < master_count; ++mi)
		for (int pi = 0; pi < points_count; ++pi)
			if ( indx( mi, pi) < (int) cont.size() )
			{
				KPOINT p = get_point(mi, pi);
				pd.put_point(mi, pi, p);
			}

	cont.assign(pd.cont.begin(), pd.cont.end());
	
	changes.resize(cont.size());	all_changed(true);
}

void CPointDump::config(int _master_count, int _points_count)
{
	if ( (master_count == _master_count) &&	(points_count == _points_count) ) return;

	master_count = _master_count;	points_count = _points_count;

	resize();
}


bool CPointDump::copy_from(const long * ptr, size_t longs_count)
{
	if ( longs_count != 2 * cont.size() ) return false;
	

	for (unsigned i = 0; i < cont.size(); ++i)
	{
		KPOINT p (ptr[2*i], ptr[2*i + 1]);
		if ( is_ch( p, cont[i]) )	
							changes[i] = true;	
			
		cont[i] = p;		
	}
	
	return true;
}

size_t CPointDump::get_buff(const KPOINT ** pptr) const

{
	if (pptr) *pptr = GetPtr(cont);	
	return cont.size();
}

		
//bool CPointDump::get_point(int master_index, int point_index, long * px, long * py) const
const KPOINT& CPointDump::get_point(int master_index, int point_index) const
{
	return cont [indx( master_index, point_index)];
}

bool CPointDump::is_point_common(int m1,  int m2, int point_index) const
{
	if ( (m1 > master_count) && (m1 < -1) ) return false;
	if ( (m2 > master_count) && (m2 < -1) ) return false;
	
	int i1 = indx( m1, point_index);
	int i2 = indx( m2, point_index);

	return  cont [i1].valid() && cont [i2].valid() ;
}

bool CPointDump::put_point(int master_index, int point_index, KPOINT p)
{
	if ( (master_index > master_count) && (master_index < -1) ) return false;
	
	int i = indx( master_index, point_index);

	if ( is_ch( p, cont[i]) ) set_changes( master_index, point_index, true);

	cont[i] = p;
	
	return true;
}

int CPointDump::common_points_count( int master_index1, int master_index2) const
{
	int res = 0;
	for (int pi=0; pi < points_count; pi++)
	{
		if (is_point_common( master_index1, master_index2, pi) ) ++res;
	}
	return res;
}
/*
bool CPointDump::is_valid() const 
{
	return cont.size() == (master_count + 1) * points_count;
}
*/
void CPointDump::set_changes(int master_index, int point_index, bool value)
{
	changes[indx( master_index, point_index)] = value;
}

bool CPointDump::is_master_changed(int master_index) const
{
	bool res = false;

	for (unsigned pi = 0; find_valid( &pi, master_index); ++pi)
		res = res || is_point_changed ( master_index, pi);
	return res;
	
}

bool CPointDump::is_pair_changed(int mi_1, int mi_2) const
{
	bool res = false;

	for (unsigned pi = 0; find_valid( &pi, mi_1, mi_2); ++pi)
		res = res || is_point_changed ( mi_1, pi) || is_point_changed ( mi_2, pi);
	
	return res;
}

bool CPointDump::is_point_changed (int master_index, int point_index) const
{
	return changes[indx( master_index, point_index)];
}

std::tstring get_point_tag(int mi, int pi)
{
	const  size_t s = 30; _TCHAR point_tag_name[s];
	if (mi >= 0) 
		_sntprintf( point_tag_name, s, _T("master%i_point%i"), mi, pi);
	else
		_sntprintf( point_tag_name, s, _T("map0_point%i"), pi);
	return std::tstring (point_tag_name);

}

class rPoint
{
	long & rx;	long & ry;
public:
	rPoint( long & _rx, long & _ry) :rx(_rx), ry(_ry)
	{}

	 void ser(ser::CElement&  e)
	 {
		e.attr(_T("x")	, rx);
		e.attr(_T("y")	, ry);
	 }

};

void CPointDump::ser(ser::CElement& e)
{
	e.attr(_T("master_count"), master_count);
	e.attr(_T("points_count"), points_count);

	if (e.is_loading()) resize(); 
	
	for (int mi = -1; mi < master_count; ++mi)
		for (int pi = 0; pi < points_count; ++pi)
		{
			//int i = indx(mi, pi);
			
			e.ser_obj( get_point_tag( mi, pi).c_str(), cont[indx(mi,pi)]);
		}

	if (e.is_loading()) all_changed(true);
	
}

void CPointDump::old_load( CBufferInput * bi )
{
	const char * p_str = "Points=";
	
	//if (find_down("AlignPoints=")) skip(AlignPoints);
	if (!bi->find_down(p_str)) throw std::string(p_str);
	
	bi->check(p_str);

	long NP = 0;
	bi->GetLong(&NP);	points_count = NP;

	resize();

	if(points_count)
	{
		std::vector <long> Points (points_count*(master_count + 1)*2);

		for(int i=0;i<points_count;i++)
		{
			long L = -1;
			bi->check("Point#=");		
			bi->GetLong(&L);
			
			for(int j= -1; j < master_count; j++)
			{
				unsigned u = 2*indx(j,i);
				if (bi->GetLong(&L)==WRONG_FORMAT) throw std::string("loading points");
				
				Points[u] = L;

				if (bi->GetLong(&L)==WRONG_FORMAT) throw std::string("loading points");

				Points[u+1] = L;
			}
		}
		copy_from( &Points[0], Points.size());	
		all_changed(true);
	}
}


	bool CPointDump::is_interpolation_area(int mfrom, int mto, KPOINT point) const
	{	
		double f, fmin, fmax, diff;
		bool res=false;
		fmin=fmax=100;
		const double pi = 3.1415926535897932384626433832795f;
		for (int i=0; i<points_count; i++)
			if (is_point_common( mfrom, mto, i)) 
			{
				KPOINT p  =get_point( mfrom, i);
				if ((p.x == point.x) && (p.y == point.y)) 
				{ 
					res = true; break;
				}
				
				f=atan2(double(p.y-point.y), double(p.x-point.x))/(2*pi);
				if (f<0) f=f+1;
				if (fmin>10) 
				{
					fmin=f;
				} 
				else 
					if (fmax>10)  
					{
						diff=(f>fmin)?(f-fmin):(1+f-fmin);
						if (diff<0.5) 
						{	
							fmax=fmin;	fmin=f;	
						} 
						else 
						{
							fmax=f;	
						}
					} 
					else 
					{
						if (fmax>fmin) 
						{
							if ((f>fmin) && (f<fmax)) 
							{
								if (f-fmin>fmax-f) fmax=f; else fmin=f;
							}
						} 
						else 
						{
							if (f<fmax) 
							{
								if (1+f-fmin>fmax-f) fmax=f; else fmin=f;
							}
							if (f>fmin) 
							{
								if (f-fmin>1+fmax-f) fmax=f; else fmin=f;					
							}
						}
						diff=(fmax>fmin)?(fmax-fmin):(1+fmax-fmin);
						if (diff<0.5) 
						{
							res=true; break;
						}
					}
			} //if
	return res;
	}
	//--------------------------------------------------------------------
	unsigned CPointDump::find_valid( unsigned pi, int mi) const
	{
		while (!end(pi))
		{
			if (get_point( mi, pi).valid())
				break;
			++pi;
		}
		return pi;
	}
	
	unsigned CPointDump::find_valid( unsigned pi, int mi_1, int mi_2) const
	{
		while (!end(pi))
		{
			bool c1 = get_point( mi_1, pi).valid();
			bool c2 = get_point( mi_2, pi).valid();

			if (c1 && c2) break;
			++pi;
		}
		return pi;
	}
	bool CPointDump::find_valid( unsigned * pi, int mi) const
	{
		while (!end(*pi))
		{
			bool c1 = get_point( mi, *pi).valid();
			
			if (c1) return true;
			++(*pi);
		}
		return false;
	}

	bool CPointDump::find_valid( unsigned * pi, int mi_1, int mi_2) const
	{
			while (!end(*pi))
		{
			bool c1 = get_point( mi_1, *pi).valid();
			bool c2 = get_point( mi_2, *pi).valid();

			if (c1 && c2) return true;
			++(*pi);
		}
		return false;
	}

	bool CPointDump::end(unsigned pi) const
	{
		return pi >= unsigned(points_count);
	}

	unsigned CPointDump::valid_count(int mi_1, int mi_2) const
	{
		unsigned res = 0;
		unsigned pi  = 0;
		while (!end(pi))
		{
			bool c1 = get_point( mi_1, pi).valid();
			bool c2 = get_point( mi_2, pi).valid();

			if (c1 && c2) res++;
			++pi;
		}
		return res;

	}

	point_vect_t CPointDump::GetPoints(int from, int to) const
	{
		point_vect_t result;
		result.reserve (10);
		for (size_t pi=0; !end(pi); ++pi)
		{
			KPOINT p = get_point( from, pi);
			if ( p.valid() && get_point( to, pi).valid() )
				result.push_back (p);
		}
		return result;
	}
/*	
	//--------------------------------------------------------------------

	CPointDump::const_iterator CPointDump::begin(int master_index) const
	{
		return const_iterator( master_index, 0, this);
	}
	CPointDump::pair_iterator CPointDump::begin(int mi1, int mi2) const
	{
		return pair_iterator( mi1, mi2, 0, this);
	}

	CPointDump::const_iterator CPointDump::end() const
	{
		return const_iterator( -1, points_count, this);
	}

	/////// *************** iterator **************** /////////

	CPointDump::const_iterator::const_iterator 
		( int _master_index, unsigned _point_index, const CPointDump * _pd )
		: pd (_pd), master_index(_master_index), point_index(_point_index)
	{
		goto_valid(+1);
	}

	const KPOINT& CPointDump::const_iterator::operator* () const
	{
		return pd->get_point( master_index, point_index);
	}

	const KPOINT& CPointDump::const_iterator::operator[] (unsigned i) const
	{
		return pd->get_point ( master_index, i);
	}

	bool CPointDump::const_iterator::operator~ () const
	{
		return pd->is_point_changed( master_index, point_index);
	}
			
	CPointDump::const_iterator& CPointDump::const_iterator::operator++ ()
	{
		point_index++;
		goto_valid(+1);
		
		return *this;
	}
	CPointDump::const_iterator& CPointDump::const_iterator::operator-- ()
	{
		point_index--;
		goto_valid(-1);
		
		return *this;
	}
	
	bool CPointDump::const_iterator::operator== (const const_iterator& it) const
	{
		return it.point_index == point_index;
	}

	bool CPointDump::const_iterator::end() const
	{
		return point_index >= pd->points_count;
	}
	
	CPointDump::const_iterator& CPointDump::const_iterator::operator+ (unsigned n)
	{
		point_index += n;
		return *this;
	}
			
	bool CPointDump::const_iterator::operator!= (const const_iterator& it) const
	{
		return it.point_index != point_index;
	}

	bool CPointDump::const_iterator::operator< (const const_iterator& it) const
	{
		return point_index < it.point_index;
	}

	void CPointDump::pair_iterator::goto_valid(int dir) const
	{
		while (!end())  
		{
			KPOINT p = pd->get_point( master_index, point_index);
			point_index += dir;
			if (p.valid()) break;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////

	CPointDump::pair_iterator::pair_iterator( int mi1, int mi2, unsigned pi, const CPointDump * _pd)
		:const_iterator ( mi1, pi, _pd), master_index(mi2)
	{
	}

	CPointDump::const_iterator& CPointDump::pair_iterator::operator++ ()
	{
		while (!end())  
		{
			const_iterator::operator++();
			
			const KPOINT& p = pd_ptr()->get_point( master_index, index());
			if (p.valid()) break;
		}
		return *this;
	}

	CPointDump::const_iterator& CPointDump::pair_iterator::operator-- ()
	{
		while (!end())  
		{
			const_iterator::operator--();
			
			const KPOINT& p = pd_ptr()->get_point( master_index, index());
			if (p.valid()) break;
		}
		return *this;
	}

	unsigned CPointDump::pair_iterator::count() const
	{
		unsigned pi = 0;
		unsigned the_count = 0;
		unsigned total_count = pd_ptr()->PointsCount();

		while (pi < total_count)
		{
			KPOINT p1 = pd_ptr()->get_point( index(), pi);
			KPOINT p2 = pd_ptr()->get_point( master_index, pi);
			
			if (p1.valid() && p2.valid()) 	++the_count;
			++pi;
		}
		return the_count;
	}

	void CPointDump::pair_iterator::goto_valid(int dir) const
	{
		while (!end())
		{
			const_iterator::goto_valid(dir);
			const KPOINT& p = pd_ptr()->get_point( index(), pi);
			if ()
			index(index() + dir);
		}
	}*/