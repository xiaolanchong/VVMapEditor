#include "stdafx.h"
#include "get_same_obj.h"

#include "memleak.h"

	void get_same_object::do_it(
				const trans::CTransCont * t, long il, long ir,
				const CObjectRecord *Obj1, const CObjectRecord *Obj2, 
				unsigned N1, unsigned N2
			  )
	{
		//sys::CEnterCS cs_obj (CritSect);

		for (int n = 0; n < max_objects; ++n)
			SameIDs[n] = -1;

		if (N2 <= 0) return;

		if (!t) throw "trans::CTransCont ptr = NULL";
 
			for(unsigned k=0; k < N1; k++)
			{	
				//И все центры объектов пересчитываем на второй мастер
				//Вообще конечно надо не центры, надо чего-то еще; но
				//и так вроде работает...

				
				SameIDs[k] = -1;

				KPOINT res1;
				KRECT r1;
				
				r1 = rect (Obj1[k]);
				
				if ( t->get_xy(il, r1.center(), ir, &res1) < 0)	continue;
			
				for ( unsigned n=0; n < N2; ++n)
				{
					KPOINT res2;	
					KRECT r2 = rect (Obj2[n]);
					
					if ( t->get_xy(ir, r2.center(), il, &res2) < 0)	continue;

					if (r2.is_inside(res1) && r1.is_inside(res2)) 
						SameIDs[k] = n; 
				}

			}

	}