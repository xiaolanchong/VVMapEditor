#ifndef EXTENTS_H_included
#define EXTENTS_H_included

#include <vector>

#include "frames.h"
#include "kpoint.h"
#include "BufferInput.h"
#include "serializer3.h"

namespace masters
{

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
	class CExtentCont
	{
		krect_vect_t items;
		typedef krect_vect_t::iterator		iter;
		typedef krect_vect_t::const_iterator	iterc;
		
		std::vector <long> the_table;
		
		int w;		int h;	long master_id;

		void set_val(unsigned x, unsigned y, long val);
		inline unsigned indx(unsigned x, unsigned y) const;
		std::tstring tag_str;

		zipper::CBuff save_from_buff();
		void load_from_buff(zipper::CBuff b);
		
	public:
		//////////////////////////////////////////////////////
		const KRECT * ptr() const	{ return GetPtr(items); }
		size_t size() const			{ return items.size();}
		//////////////////////////////////////////////////////
		void load_from_array(unsigned array_size, const KRECT * array);
		//////////////////////////////////////////////////////
				
		CExtentCont() : w(-1), h(-1) 	{}

		long compile ( int _w, int _h);
		void clear();

		const long * table_ptr() const { return GetPtr(the_table);}
		size_t table_size() const 
		{ 
			return the_table.size();
		}

		int width () const { return w;}
		int height () const { return h;}

		void set_mastert_id(long mid);

		long get_val(unsigned x, unsigned y) const;

		void old_load(CBufferInput& Input, int _w, int _h);

		void old_load_table(CBufferInput& Input);

		void old_save_table(CBufferInput& Input);

		//////////////////////////////////////////////////////
		void ser(ser::CElement& e);
		//////////////////////////////////////////////////////
		//void tag(const _TCHAR * t) { tag_str = t;}
	};
//////////////////////////////////////////////////////////////
}

#endif
