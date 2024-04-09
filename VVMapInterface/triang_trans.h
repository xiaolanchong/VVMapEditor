#ifndef _TRIANG_TRANS_H_included_
#define _TRIANG_TRANS_H_included_

#include "triang.h"

namespace trans
{
	

	class CTriangTrans :public CTrans
	{	
		
	public:

		CNeuroTrans(  const KSrcDataContext &dc, bool _enabled) 
			:CTrans (dc), enabled(_enabled), ready (false)
		{}
		//////////////////////////////////////////////
		virtual long compile( KSrcDataContext& src_data, CTransClip * the_clip ); 
		//////////////////////////////////////////////
		virtual bool get_res ( KPOINT Src, KPOINT * pRes) const;

		virtual bool is_ready() const;
		virtual bool is_enabled() const;
		//////////////////////////////////////////////
		virtual void ser(ser::CElement& e);
		virtual long type_id() const;
		virtual const _TCHAR * trans_name() const;
		
		virtual void bind_opts	(opts::KDialog * d, opts::Itm * i);
	}
}

#endif