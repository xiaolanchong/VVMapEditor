#include "stdafx.h"	
#include "VVMReal1.h"
#include "poly.h"

namespace 
{ 
	template<class T> T sq(T x) {return x*x;} 
}	
	long CVVMReal::TranslatePolygons
				(
					long indexfrom, 
					long indexto, 
					const std::vector<RECT>& rects,
					std::list < std::vector<POINT> >& results
				)
	{
		results.clear();

		if (!p_src_data->is_master (indexfrom) || !p_src_data->is_map (indexto))
			return ERR_INVALID_INDEX;

		// поиск подходящего преобразования
		trans::CTransClip * t = p_trans_cont->find(indexfrom, p_src_data->correct_index(indexto));

		if (!t) return ERR_TRANSITION_UNDEFINED;

		
		size_t rN = rects.size();

		bool src_poly_empty = t->get_source_vis_poly().count() > 2;
		bool map_poly_empty = t->get_vis_poly().count() > 2;
		if (map_poly_empty != src_poly_empty)
			t->compile_vis_poly( *p_src_data );

		const CPoly& src_poly = t->get_source_vis_poly();
		size_t count = src_poly.count();
		std::vector<CPoly> polygons ( rN, t->get_source_vis_poly() );
		
		// вычленяем многоугольники-пересечения
		
		for (size_t n=0; n < rN; ++n)
		{
			polygons[n].cut_rect(KRECT(rects[n]));
		}
		
		// переносим точки на карту
		for (std::vector<CPoly>::const_iterator pi = polygons.begin(); pi != polygons.end(); ++pi)
		{
			// копируем результат
			results.push_back(std::vector<POINT>());

			if (pi->count() == 0) continue;

			// полигон на карте - временный объект
			CPoly temp_poly;

			// массив точек на мастере
			const KPOINT * ptr = pi->data();
			size_t pN = pi->points_count();

			// переносим точки на карту. заносим их в  temp_poly
			for (size_t n = 0; n < pN; ++n)
			{
				KPOINT res;
				long ok = t->get_res( ptr[n], &res);
				if (ok == ERR_NO_ERROR)
					temp_poly.push(res);
			}

			// усекаем полигон по границам карты
			long map_w = p_src_data->get_map()->width();
			long map_h = p_src_data->get_map()->height();
			temp_poly.cut_rect( KRECT( map_w, map_h) );

			if (temp_poly.points_count() > 0)
			{
				const POINT *begin = temp_poly.data(), *end = begin + temp_poly.points_count();
				results.back().assign ( begin, end );
			}
			
		//	m_translated_polygons[indexfrom] = results;
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////
// Вспомогательные функции
/////////////////////////////////////////////////////////////////////////////////////////////
namespace InscribeRectHelper
{
	struct InscribeInfo
	{
		enum {NONE, MOVED, DECREASED, INCREASED};
		RectF result;
		RectF last_result;
		MyFloat ratio;
		bool first_time;
		int last_index;
		bool finished;
		int last_action;
		int mode;
		MyFloat S0;
		MyFloat Surf[8];

		InscribeInfo() {	reset();	}
		bool all_inside() const;
		void set_result(RectF r);
		pointf_vect temp;
		void reset() 
		{
			ratio= 0;
			first_time = true;
			last_index = -1;
			S0 = 0;
			finished = false;
			last_action = NONE; 
			mode = NONE;
			zero( result );
			zero( last_result );
		}
	};


	RectF GrowRect(RectF r, MyFloat ratio, int grow = 1)
	{
		r.left	-= grow;	r.right += grow;
		MyFloat dh = (r.right - r.left) / ratio / 2;
		MyFloat y0 = (r.top + r.bottom) / 2;
		r.top = y0 - dh;
		r.bottom = y0 + dh;
		return r;
	}

	RectF MoveRect(RectF r, int dx, int dy)
	{
		r.left	+= dx;	r.right	+= dx;
		r.top	+= dy;	r.bottom	+= dy;
		return r;
	}

	RectF pt_rect(int cx, int cy, int w, int h)
	{
		RectF r;
		r.left = MyFloat(cx - w/2);		r.top = MyFloat(cy - h/2);	
		r.right = MyFloat(cx + w/2);	r.left = MyFloat(cy + h/2);
		return r;
	}

	MyFloat RectSurf(RectF r)
	{
		return abs ((r.right - r.left) * (r.bottom - r.top));
	}

	bool InscribeInfo::all_inside() const
	{
		MyFloat s = RectSurf(result);
		return S0 >= s - 0.1;
	}

	void InscribeInfo::set_result(RectF r)
	{
	#ifdef TRY_INT
		result = RECT(r);
	#else
		result = r;
	#endif

		if (all_inside())
			last_result = r;
	}


	void InscribeRectStep (const CPoly& p, int w, int h, InscribeInfo& ii );


	void InscribeRect (const CPoly& p, int desired_width, int desired_height, RECT& result  )
	{
		InscribeInfo ii;

		static const int MAX_ITERS = 100000;
		for (int n=0; n < MAX_ITERS && !ii.finished; ++n)
			InscribeRectStep ( p, desired_width, desired_height, ii );

		result = ii.result;
	}

	static const int move_N = 8;
	static const int move_x[move_N] = { -1,  -1,  0,  +1,  +1,  +1,  0, -1};
	static const int move_y[move_N] = {  0,  -1, -1,  -1,   0,  +1, +1, +1};

	void InscribeRectStep (const CPoly& p, int w, int h, InscribeInfo& ii )
	{
		if (ii.first_time)
		{
			ii.first_time = false;
			ii.ratio = MyFloat (w) / h;
			//POINT cmp = p.MassCenterByPerimeter();
			PointF cm = p.MassCenterBySurface();
			
			ii.result.left		= cm.x - w / 2;
			ii.result.right		= cm.x + w / 2;
			ii.result.top		= cm.y - h / 2;
			ii.result.bottom	= cm.y + h / 2;

			ii.S0 = p.IntersectSurf(ii.result);
			ii.finished = false;
			ii.last_action = InscribeInfo::NONE;
			ii.mode = InscribeInfo::NONE;
			for (int n=0; n<move_N; ++n)ii.Surf[n]=0;
		}
		else
		{
		
			for (int n=0; n<move_N; ++n)
			{
				if (n == ((ii.last_index + move_N/2) % move_N)) { ii.Surf[n] = 0; continue; }
				ii.Surf[n] = p.IntersectSurf( MoveRect(ii.result, move_x[n], move_y[n]) );
			}
		
			int max_index = -1;
			MyFloat max_dS = 0;
			MyFloat * pSmax = std::max_element ( ii.Surf, ii.Surf + move_N );
			
			// двигаем только в том случае, если это приводит к улучшению
			if (*pSmax > ii.S0)
			{
				ii.last_index = (int) std::distance (ii.Surf, pSmax);
				ii.S0 = ii.Surf[ii.last_index];
				ii.set_result ( MoveRect(ii.result, move_x[ii.last_index], move_y[ii.last_index]) );
				
				ii.last_action = InscribeInfo::MOVED;
				return;
			}
	/*
			if (*pSmax > ii.S0 && ii.all_inside())
			{
				ii.first_time = true;
				ii.finished = true;
				//ii.result = ii.last_result;
				return;
			}
	*/
			// сюда попадаем если двигаться некуда

			// если выходим за границы (но все-таки пересекаемся), наша стратегия - уменьшение
			if (ii.mode != InscribeInfo::INCREASED && (ii.mode == InscribeInfo::DECREASED || !ii.all_inside() && ii.S0 > 0))
			{
				if (ii.all_inside())
				{
					// зашли во внутрь.. меняем стратегию на увеличение
					ii.last_result = ii.result;
					ii.mode = InscribeInfo::INCREASED;
					return;
				}

				ii.mode = InscribeInfo::DECREASED;
				ii.set_result ( GrowRect ( ii.result, ii.ratio, -1 ) );
				ii.S0 = p.IntersectSurf( ii.result  );
				ii.last_action = InscribeInfo::DECREASED;
				return;
			}

			// 
			if (ii.mode == InscribeInfo::INCREASED || (ii.all_inside() || ii.S0 == 0))
			{
				if (ii.S0 > 0)
					ii.mode = InscribeInfo::INCREASED;

				if (!ii.all_inside())
				{
 					ii.first_time = true;
					ii.finished = true;
					ii.result = ii.last_result;
					return;
				}

				RectF r1 = GrowRect ( ii.result, ii.ratio );
				MyFloat S = p.IntersectSurf( r1 );
				//MyFloat S1 = p.IntersectSurf( CPoly (r1) );

				ii.S0 = S;
				ii.set_result ( r1 );
				ii.last_action = InscribeInfo::INCREASED;
 
				return;	
			}
		}
	}

}
	
//////////////////////////////////////////////////////////////////////////
	long CVVMReal::InscribeRect(int master_index, long w, long h, RECT& result )

	{
		if (!p_src_data->is_master (master_index))
			return ERR_INVALID_INDEX;

		// поиск подходящего преобразования
		trans::CTransClip * t = p_trans_cont->find(master_index, p_src_data->map_index());

		if (!t) return ERR_TRANSITION_UNDEFINED;

		InscribeRectHelper::InscribeRect ( t->get_vis_poly(), w, h, result  );
		
		return ERR_OK;
		
	}
