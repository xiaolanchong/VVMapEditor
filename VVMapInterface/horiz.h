
#ifndef _HORIZ_H_included_
#define _HORIZ_H_included_

#include <vector>
#include "kpoint.h"

namespace trans
{
	class CHorizont
	{
		struct point_pair 
		{
			KPOINT p1; KPOINT p2;
			point_pair() : p1 (-1,-1), p2 (-1,-1) {}
		};
		std::vector <point_pair> pairs;
	public:
		void clear()
		{
			pairs.clear();
		}
		void finalize()
		{
			if (pairs.empty()) return;
			point_pair& first = pairs.front();
			point_pair& last  = pairs.back();

			if (!first.p1.not_negative()) { first.p1 = last.p1; pairs.erase(pairs.end() - 1); }
			if (!first.p2.not_negative()) { first.p2 = last.p2; pairs.erase(pairs.end() - 1); }
			
		}

		void push_enter_point (KPOINT p)
		{
			if (pairs.empty() || pairs.back().p1.not_negative())	pairs.push_back(point_pair());
			
			pairs.back().p1 = p;
		}
		void push_leave_point (KPOINT p)
		{
			if (pairs.empty() || pairs.back().p2.not_negative())	pairs.push_back(point_pair());

			pairs.back().p2 = p;
		}

		const KPOINT * get_data_ptr () const { return pairs.size() ? &pairs[0].p1 : 0;}
		size_t points_count () const 
		{ 
//			size_t s = pairs.size();
			return 2 * pairs.size();
		}
	};
}

#endif