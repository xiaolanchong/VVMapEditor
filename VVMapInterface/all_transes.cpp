#include "stdafx.h"
#include "all_transes.h"

#include "projective_trans.h"
#include "neural_trans.h"

#include "thread_safe_global.h"
#include "memleak.h"

namespace trans
{
		void CTransCont::create_log_win() const
		{
		//	if (!lw)
		//		lw = new CLogWin();
		}

		void CTransCont::delete_log_win() const
		{
		//	if (lw) delete lw; lw = 0;
		}

		CTransCont::CTransCont(KSrcDataContext& _data) : src_data(_data), make_log(false),/* lw(0), */
		last_trans (_T("-- NO TRANS --"))
		{}

		CTransCont::~CTransCont() 
		{ 
			delete_log_win();
			clear();
		}

		CTransClip * CTransCont::find(int ifrom, int ito)// const
		{
			
			for (iter i = items.begin(); i != items.end(); ++i)
				if ( CTransClip *tc = (i->ptr) )
				{
					if ( (tc->ifrom() == ifrom) && (tc->ito() == ito) )
																	return tc;
				}
			return 0;
		}

		const CTransClip * CTransCont::find(int ifrom, int ito) const
		{
			
			for (iterc i = items.begin(); i != items.end(); ++i)
				if ( const CTransClip *tc = (i->ptr) )
				{
					if ( (tc->ifrom() == ifrom) && (tc->ito() == ito) )
																	return tc;
				}
			return 0;
		}
//--------------------------------------------------
		long CTransCont::get_xy_pure(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const
		{
			if (const CTransClip * t = find(ifrom, ito))
			{
				//last_trans = t->last_trans_name();
				return t->get_res( Src, pRes);
			}
			else return ERR_TRANSITION_UNDEFINED;
		}
//-------------------------------------------------
		
//////////////////////////////////////////
		long CTransCont::__get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const
		{
			int n = 0;
			KPOINT res;
			long lres = 0;
			
			for (unsigned i=0; i < items.size(); ++i)
			if (items[i].ptr->is_this( ifrom, ito))
			{
				n++;
				if (!items[i].ptr->is_valid()) 
							return ERR_TRANSITION_UNDEFINED;

				lres = items[i].ptr->get_res( Src, &res);
				if (lres == ERR_NO_ERROR)
				{
					
					last_trans = items[i].ptr->last_trans_name();

					if (routes.empty()) break;
				
					//continue;
					for (unsigned ii=0; ii < routes[i].size(); ii++)
					{
						two_indexes two_i = routes[i][ii];
						if (!two_i.ready()) continue;
						
						unsigned i1 = two_i.i1;
						unsigned i2 = two_i.i2;
						
						int i1_to = items[i1].ptr->ito();
						KPOINT pp;	KPOINT rr;
						if	(
							 (items[i1].ptr->get_res( Src, &pp)== ERR_NO_ERROR)
							 &&
							 (src_data.masters_cont.items[i1_to].inside(pp.x, pp.y))
							 &&
							 (items[i2].ptr->get_res(  pp, &rr)== ERR_NO_ERROR)
							)
						{  
							res += rr; n++;
						}
					
					}
					break;
				}
			}
			
			if (!n)	
							return ERR_TRANSITION_UNDEFINED;

			if (lres !=  ERR_NO_ERROR) 
							return lres;

			pRes->x = (long) (double(res.x) / n);
			pRes->y = (long) (double(res.y) / n);

			return lres;
		}

		long CTransCont::_get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const
		{
			long r = __get_xy( ifrom, Src, ito, pRes);
			///////////////////Обработка ошибок///////////////////////

			if (r != ERR_NO_ERROR) return r;
			
			//if (src_data.vismap_enabled)
				if ((r == ERR_TRANSITION_INFINITY) || (r == ERR_NO_ERROR))
					
					if ( src_data.is_map(ifrom) && (src_data.iz_cont.is_invis( ito, Src)) )	
								
						return ERR_POINT_INVISIBLE_SRC;
		
		//----------------------
			if (src_data.vismap_enabled)
				if ( src_data.is_map(ito) && (src_data.iz_cont.is_invis( ifrom, *pRes)) )
				{
					src_data.get_map()->vmap.BringToVis( ifrom, &(pRes->x), &(pRes->y) );

					if (src_data.pinfo->is_editor()) 
						return ERR_POINT_INVISIBLE;
					else
						return ERR_NO_ERROR;
				}
		//----------------------
			if (!src_data.is_inside( ito, *pRes) )
			{
				return ERR_POINT_OUT_OF_RECT;
			}
		//----------------------
			if (!src_data.is_inside( ifrom, Src) )
			{
				return ERR_POINT_OUT_OF_RECT_SRC;
			}
		//----------------------

			return ERR_NO_ERROR;
		}

		long CTransCont::get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const
		{
			long res = _get_xy( ifrom, Src, ito, pRes);
			 ///// log-list/////////////////////
			/*
			if (make_log) 
			{
				create_log_win();
//				lw->show(true);
//				log_text ( res, ifrom, Src, ito, pRes);
			}
			else
			{
				delete_log_win();
			}
			*/

			return res;
		}

		static std::tstring err_str(long _err);
		static std::tstring point_str(KPOINT p);
		static std::tstring from_to_str(long from, long to);


		void CTransCont::log_text (long getxy_res, int ifrom, KPOINT Src, int ito, KPOINT * pRes) const
		//long CTransCont::get_res_text ( int ifrom, KPOINT Src, int ito, KPOINT * pRes, std::tstring * rs ) const
		{
		/*	//const _TCHAR * i = last_trans;
			std::tstring rs = std::tstring(last_trans);
			rs = rs + from_to_str( ifrom, ito);
			rs = rs + std::tstring(_T("src(")) + point_str (Src);
			rs = rs + std::tstring(_T(") res("));
			rs = rs + point_str (*pRes);
			rs = rs + std::tstring(_T(") "));
			rs = rs + err_str(getxy_res);


			lw->text(rs.c_str());*/
		}
//////////////////////////////////////////										
//--------------------------------------------------
		long CTransCont::compile( CProgress * prog)
		{
			long res = 0;
			try 
			{
				res = compile_trans(prog);
				if (res != ERR_NO_ERROR) throw;

				fill_routes();

				res = compile_vis_maps(prog);
				if (res != ERR_NO_ERROR) throw;

				set_all_ready();
				
				src_data.iz_cont.all_changed(false);
				src_data.points_cont.all_changed(false);
				
				_compile_all_next_time = false;
				calc_quality();
				return ERR_NO_ERROR;
			}
			catch (...)
			{
				clear();
				return res;
			}
		}

		void CTransCont::set_all_ready()
		{
			for (iter i = items.begin(); i != items.end(); ++i)
															i->ptr->set_ready();
		}

		void CTransCont::shadow_compile_trans()
		{
			long dummy1 = 0; long dummy2 = 0;

			CProgress p( dummy1, dummy2);
						
			if (compile_trans(&p) == ERR_NO_ERROR)	fill_routes();
		}
//--------------------------------------------------

		std::tstring CTransCont::i2name(int master_i) const
		{
			unsigned s_id = IDS_UNKNOWN;
			bool _map = src_data.is_map(master_i);
			bool _master = src_data.is_master(master_i);
			
			if (_master)	s_id = IDS_MASTER;
			if (_map)		s_id = IDS_MAP;
			std::tstring res_s = sys::get_res_string(s_id);

			if (_master)
			{
				int id = src_data.masters_cont.items[master_i].getID();
				_TCHAR buf[20]={0};
				_sntprintf ( buf, 20-1, _T(" id %i"), id);
				return (res_s + buf);
			}
			return res_s;
		}
//--------------------------------------------------
		std::tstring CTransCont::gen_trans_lb_label( int _from, int _to) const
		{
			return i2name(_from) + _T(" -> ") + i2name(_to);
		}

//--------------------------------------------------
		long CTransCont::compile_trans( CProgress * prog)
		{
		//	if (ready()) return;
			int all_trans = items.size();

			int compiled_trans = 0;
			CTransFab fab (src_data);

			for (iter i = items.begin(); i != items.end(); ++i)
				{
					if (prog->stop()) return ERR_PROCESS_STOPPED;
					prog->set_progress( 0, double(compiled_trans) / all_trans);
					
					i->ptr->compile(src_data);
				}
			return ERR_NO_ERROR;
		}
//--------------------------------------------------
		void CTransCont::compile_vis_map1( iter i)
		{
			int mi = i->ptr->ifrom();

			if (!src_data.is_map(i->ptr->ito())) return;
			if (src_data.maps_cont.items.empty()) return;
	
			long w = src_data.masters_cont.items[mi].width();
			long h = src_data.masters_cont.items[mi].height();
			masters::CMap * m = &(src_data.maps_cont.items[0]);

			masters::map_filler mf(m->vmap, w, h, mi);

			if (i->ptr->is_valid()) 

				while (!mf.end())
				{
					KPOINT p;
					bool put = (__get_xy(i->ptr->ifrom(), mf.cur_pos(), i->ptr->ito(), &p) == ERR_NO_ERROR);

					put = put && !(src_data.iz_cont.is_invis( mi,p));

					mf.put_point( put, p);
				}

		//*/
		}
//--------------------------------------------------
		long CTransCont::compile_vis_maps( CProgress * prog)
		{
			if (!src_data.vismap_enabled) 
			{ 
				src_data.maps_cont.items[0].vmap.release(); 
				return 0;
			}

			//подсчитать сколько карт необходимо построить
			int all_count = 0;
			for (iterc i = items.begin(); i != items.end(); ++i)
				
				if (src_data.is_map(i->ptr->ito()) && src_data.is_master(i->ptr->ifrom()))
					
					if (i->ptr->is_valid())
						if (!i->ptr->ready() || iz_changed(i->ptr->ifrom())) all_count++;
			
			//обнулить все карты видимости
			//for (unsigned mi = 0; mi < src_data.maps_cont.Count(); mi++)
			
//			if (!all_count) return 0;

			src_data.maps_cont.items[0].init_vis_map();
			//очищает карту выдимости только в том случае, если изменились размеры карты


			bool vis_map_changed = false;
			int compiled_count = 0;
			for (iter it = items.begin(); it != items.end(); ++it)
				if (src_data.is_map(it->ptr->ito()) && src_data.is_master(it->ptr->ifrom()))
				{
					if (prog->stop()) return ERR_PROCESS_STOPPED;

					if (it->ptr->ready() && !iz_changed(it->ptr->ifrom()) && it->ptr->is_valid() && !_compile_all_next_time) continue;
					
					compile_vis_map1(it);	compiled_count++;
					vis_map_changed = true;
					
					prog->set_progress( 1, double(compiled_count) / all_count);
				}
			if (vis_map_changed) 
				src_data.maps_cont.items[0].ProcessVisBM(src_data.maps_cont.colors);
			return 0;
		}
//--------------------------------------------------
		void CTransCont::reserve_transes(bool reset_all)
		{
			const int min_points = 4;
			_compile_all_next_time = _compile_all_next_time || reset_all;
			CTransFab fab (src_data);

			if (reset_all) 
				clear();

			//size_t nm = src_data.masters_cont.Count();
			items.reserve(20);


			for (int i = src_data.master_begin(); i < src_data.master_end(); ++i)
				for (int j = src_data.master_begin(); j < src_data.master_end(); ++j)
				//по всем мастерам
				{
					if (i == j) continue;
					//если есть 4 точки - должна быть кассета с преобразованиями
					if (src_data.points_cont.common_points_count(i, j) >= min_points)
					{
						//ищем
						CTransClip * tc = find( i, j);
						if (tc)
						{
							//нашли, дозаряжаем недостающие
							fab.charge_clip( tc ); 
						}
						else
						{
							//создаем
							items.push_back(CTransClipPtr());
							tc = items.back().ptr;
							std::tstring s = gen_trans_lb_label( i, j);
							tc->set_indexes( i, j, s.c_str());
							//заряжаем
							fab.charge_clip( tc );
						}
						bool valid = !reset_all && tc->is_valid();
						tc->set_valid(valid);
					}
					else 
					{
						if (CTransClip * tc = find( i, j)) tc->set_valid(false);
					}
				}
			for ( iter it = items.begin(); it != items.end(); ++ it)
			{
				//bool c1 = src_data.points_cont.is_master_changed(  ); 
				//bool c2 = src_data.points_cont.is_master_changed( it->ito() );|| c2	
				int _from = it->ptr->ifrom();
				int _to = it->ptr->ito();
				bool c1 = src_data.points_cont.is_pair_changed( _from, _to);
				if (it->ptr->is_valid() && (_compile_all_next_time || c1 )) it->ptr->reset();
			}
			_compile_all_next_time = false; 
		}

		bool CTransCont::iz_changed( long master_index) const
		{
		//	long _ID = src_data.get_master(master_index)->getID();
			return src_data.iz_cont.is_changed_for_master(master_index);
		}
//--------------------------------------------------		
		void CTransCont::bind_opts(opts::KDialog * d, opts::Itm * i)
		{
			d->bind_vector( i, IDC_TRANS_LIST, &items);
			d->bind( i, IDC_LOG_WINDOW, &make_log);
		}
//--------------------------------------------------
		void CTransCont::clear()
		{
			for (iter i= items.begin(); i != items.end(); ++i)
				i->ptr->clear();
			if (!items.empty()) items.clear();
		}


		void CTransCont::fill_routes()
		{
			routes.resize(items.size());
			int imap = src_data.map_index();
			for (unsigned i=0; i < items.size(); ++i)
				if (items[i].ptr->ito() == imap)
				{
					int _from = items[i].ptr->ifrom();
					std::vector <two_indexes> v;
					
					for (unsigned i1=0; i1 < items.size(); i1++)
					if ((items[i1].ptr->ifrom() == _from) && (items[i1].ptr->ito() != imap))
						{
							int master_intrm = items[i1].ptr->ito();
							for (unsigned i2=0; i2 < items.size(); i2++)
							{
								if (items[i2].ptr->ifrom() != master_intrm)		
																	continue;
								if (items[i2].ptr->ito() != imap)		continue;
								if (i1 == i2)						continue;
							
								v.push_back(two_indexes(i1, i2)); 
								goto next; 
							}
							
						}
					
				next:
					routes[i] = v;
				}

		}

		ThreadSafeGlobal<CreateInfo> g_create_data;

		void CTransCont::ser(ser::CElement& e)
		{
			if (e.is_loading())
			{
				CreateInfo ci = g_create_data.get();
				ci.fab = new CTransFab(src_data);
				g_create_data.write(ci);
				
			}
			e.ser_vect(_T("TransClip"), items);

			if (e.is_loading())
			{
				CreateInfo ci = g_create_data.get();
				if (ci.fab) delete ci.fab;	ci.fab = 0;
				g_create_data.write(ci);
			}
			
			if (e.is_loading()) 
			{ 
				fill_routes();
				calc_quality();
			}
		}

		void CTransCont::calc_quality()
		{
			for (iter i = items.begin(); i != items.end(); ++i)
				if( CTransClip * t = (i->ptr) )
						t->calc_quality(src_data);
				
		}

		void CTransCont::load_old_opts( CBufferInput& Input)
		{
				bool loaded = false;
				trans::CVVMOptions old_opts;
				
				if(Input.version>10)
				{
							
					Input.goto_text("OPTIONS");
					Input.check("OPTIONS");

					const char * ch = static_cast <char*> (Input.Buffer+Input.Pointer);
					long L = old_opts.old_old_load (ch);
					Input.Pointer+=L;
					Input.skip();
					loaded = true;
				}

				if(Input.version>15)
				{
							
					Input.goto_text("Options2");
					Input.check("Options2");

					const char * ch = static_cast <char*> (Input.Buffer+Input.Pointer);
					long L = old_opts.old_load (ch);
					Input.Pointer+=L;
					Input.skip();
					loaded = true;
				}

				if (loaded)
					for (iter i = items.begin(); i != items.end(); ++i)
					{
						CProjectiveTrans * pt = dynamic_cast <CProjectiveTrans *> (i->ptr->get_trans(proj_str));
						if (pt) pt->set_opts_old(&old_opts);
					}

		}

		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////

		void KTransPtr::ser(ser::CElement& e)	
		{ 
			std::tstring s;
				
			const _TCHAR the_tag[] = _T("trans_name");
				
			if (e.is_loading())
			{
				release_trans();
				e.attr( the_tag, s);
				CreateInfo ci = g_create_data.get();
				ptr = ci.fab->create(s.c_str(), ci.from, ci.to);
				if (!ptr) 
					throw std::string( std::string ("invalid_type - ") + sys::ANSI_str(s.c_str()));
			}
			else
			{
				if (!ptr) return;
				s = std::tstring(ptr->trans_name());
				e.attr( the_tag, s);
			}
				ptr->ser(e);
		}

		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////

	static std::tstring point_str(KPOINT p)
	{
		const size_t s = 50; _TCHAR b[s];
		int x = p.x; int y = p.y; 
		_sntprintf( b, s, _T(" %i, %i"), x, y);
		return std::tstring(b);
		
	}

	static std::tstring from_to_str(long from, long to)
	{
		const size_t s = 50; _TCHAR b[s];
		int x = from; int y = to; 
		_sntprintf( b, s, _T(" from = %i, to = %i "), x, y);
		return std::tstring(b);
	}


	static std::tstring err_str(long _err)
	{
		switch (_err)
		{
		case ERR_INVALID_MASTERS:		return std::tstring(_T("ERR_INVALID_MASTERS"));
		case ERR_MAP_NOT_COMPILED:		return std::tstring(_T("ERR_MAP_NOT_COMPILED"));
		case ERR_TRANSITION_UNDEFINED:	return std::tstring(_T("ERR_TRANSITION_UNDEFINED"));
		case ERR_POINT_INVISIBLE_SRC:	return std::tstring(_T("ERR_POINT_INVISIBLE_SRC"));
		case ERR_TRANSITION_INFINITY:	return std::tstring(_T("ERR_TRANSITION_INFINITY"));
		case ERR_POINT_OUT_OF_RECT:		return std::tstring(_T("ERR_POINT_OUT_OF_RECT"));
		case ERR_POINT_OUT_OF_RECT_SRC:	return std::tstring(_T("ERR_POINT_OUT_OF_RECT_SRC"));
		
		case ERR_POINT_INVISIBLE:		return std::tstring(_T("ERR_POINT_INVISIBLE"));
		case ERR_NO_ERROR:				return std::tstring(_T("ERR_NO_ERROR"));		
		}
		return  std::tstring(_T("unknown"));
	}

	
}