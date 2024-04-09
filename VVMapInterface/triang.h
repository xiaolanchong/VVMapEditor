

#ifndef TRIANG_H_included
#define TRIANG_H_included


#include <list>
#include <math.h>



#include "trans.h"
#include "triangulation.h"

namespace trans
{
	enum
	{
		triang_id = 1003,
	};

	extern const _TCHAR * triang_str;

	


	/////////////////////////////////////////////////////////
	

	class CTriangTrans :public CTrans, triangulation::Triangulation
	{
		const CPointDump * points;
		

		//------------------------------------------------------------------
		bool ready;		bool enabled;

		long compile(); 
	/////////////////////////////////////////////////////////////////////
	public:
		//CTriangTrans(const CPointDump * _points);
		CTriangTrans(  const KSrcDataContext &dc, long _from, long _to, bool _enabled) 
				:CTrans (dc, _from, _to), enabled(_enabled), ready (false), points(&(dc.points_cont)) 
		{}
	//////////////////////////////////////////////
		virtual long compile( KSrcDataContext& src_data, CTransClip * the_clip ) { return compile(); }
		//////////////////////////////////////////////
		virtual bool get_res ( KPOINT Src, KPOINT * pRes) const;

		virtual bool is_ready() const	{ return ready;	}
		virtual bool is_enabled() const 
		{ 
			return enabled;
		}
		virtual void reset() { ready = false;}
	//////////////////////////////////////////////
		
		virtual long type_id() const { return triang_id;} 
		virtual const _TCHAR * trans_name() const { return triang_str;}
			
		virtual void bind_opts	(opts::KDialog * d, opts::Itm * i);
		virtual void ser(ser::CElement& e);

		virtual CTrans * create_copy() const { return new CTriangTrans(*this);}
	};
}

#endif