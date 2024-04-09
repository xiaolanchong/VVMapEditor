#include "stdafx.h"
#include "trans.h"

#include "projective_trans.h"
#include "neural_trans.h"
#include "triang.h"

namespace trans
{

	void CTransFab::add_trans(CTransClip * tc, long trans_id)
	{
		if (tc->get_index(trans_id) < 0) 
				if (CTrans * p = create(trans_id, tc->ifrom(), tc->ito()))
						tc->push(p);
	}

	void CTransFab::charge_clip (CTransClip * tc)
		{
	
				add_trans( tc, projective_id );
				add_trans( tc, neural_id );
				add_trans( tc, triang_id );
			//	tc->set_valid(false);
	
		}
	///////////////////////////////////////////////////////////////
		CTrans * CTransFab::create(int trans_type_id, long _from, long _to)
		{
			switch (trans_type_id)
			{
			case projective_id:
				return dynamic_cast <CTrans *> (new CProjectiveTrans ( dc, _from, _to, true) );

			case neural_id:
				return dynamic_cast <CTrans *> (new CNeuroTrans ( dc, _from, _to, false) );

			case triang_id:
				return dynamic_cast <CTrans *> (new CTriangTrans ( dc, _from, _to, true) );
			}
			return 0;
		}

		CTrans * CTransFab::create(const _TCHAR * type_name, long _from, long _to)
		{
			if (std::tstring(type_name) == std::tstring(proj_str)) 
				return dynamic_cast <CTrans *> (new CProjectiveTrans ( dc, _from, _to, false) );
			
			if (std::tstring(type_name) == std::tstring(neur_str)) 
				return dynamic_cast <CTrans *> (new CNeuroTrans ( dc, _from, _to, false) );

			if (std::tstring(type_name) == std::tstring(triang_str)) 
				return dynamic_cast <CTrans *> (new CTriangTrans ( dc, _from, _to, true) );

			return 0;
		}
///////////////////////////////////////////////////////////////


}