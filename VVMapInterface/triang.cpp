#include "stdafx.h"
#include "triang.h"

#pragma warning (disable : 4786)

inline double sq(double x) { return x*x;}
inline i64 sq(i64 x) { return x*x;}



namespace trans
{

	const _TCHAR * triang_str = _T("triangulation_trans");

	

	long CTriangTrans::compile()
	{
		
		try 
		{ 
			point_vect_t from_points = points->GetPoints (from(), to()), to_points = points->GetPoints (to(), from());
			build_all( from_points );
			
			typedef triangulation::triang_vect_t tr_vect;
			for (tr_vect::iterator i = triang_vect().begin(); i != triang_vect().end(); ++i)
			{
				KPOINT f[3];	KPOINT t[3];
				for (int n=0; n<3; ++n)
				{
					unsigned ind = i->index[n];
					f[n] = from_points [ind];
					t[n] = to_points [ind];
				}
				
				i->trans_matr.calc( f, t);
			}

			ready = true;
			
			return 0;
		}
		catch(triangulation::not_enough_points&)	
		{
			ready = false; return 1;
		}

	}
	//////////////////////////////////////////////
	bool CTriangTrans::get_res ( KPOINT Src, KPOINT * pRes) const
	{
		if (!enabled) return false;

		if (!points->is_interpolation_area( from(), to(), Src)) return false;

		Matr3x2 m;
		if (GetMatrForTriang( Src, &m))
		{
			if (m.ready)
			{
				*pRes = m.mult(Src);
				return true;
			}
		}
		return false;
		
	}

	void CTriangTrans::bind_opts (opts::KDialog * d, opts::Itm * i)
	{
		d->bind( i, IDC_TRIANG_ENABLED, &enabled);
	}

	void CTriangTrans::ser(ser::CElement& e)
	{
		e.attr( _T("enabled"), enabled);
		e.attr( _T("ready"), ready);	
		
		//------------------------------------------------------------------
		if (e.is_loading())	compile();
		else
			e.ser_vect( _T("Triangle"), triang_vect());
	}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
	
	

}//namespace