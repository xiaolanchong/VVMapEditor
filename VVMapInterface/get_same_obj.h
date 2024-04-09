
#ifndef _GET_SAME_OBJECT
#define _GET_SAME_OBJECT

#include "kpoint.h"
#include "all_transes.h"

#include "../MultiCamGlobals/messages.h"

	class get_same_object
	{
	public:
		enum { max_objects = 100};
	private:
		long SameIDs[max_objects];
		KRECT rect(const CObjectRecord& o) const { return KRECT ( o.xmin, o.ymin, o.xmax, o.ymax); }
	public:
		void do_it(
					const trans::CTransCont * t, long il, long ir, 
					const CObjectRecord *Obj1, const CObjectRecord *Obj2, 
					unsigned N1, unsigned N2
				  );

		const long * ptr() const { return SameIDs; }
	};

#endif