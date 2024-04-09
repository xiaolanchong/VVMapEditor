#include "stdafx.h"
#include "trans.h"
#include "thread_safe_global.h"

namespace trans
{
/////////////////////////////////////////////////////////////////////
//	CErrMap err_map;
/////////////////////////////////////////////////////////////////////
	
	void CTransClip::push(CTrans * tr)
	{
		items.reserve(3);
		items.push_back( KTransPtr (tr) );
		set_not_ready ();
	}
			
	
	int CTransClip::get_index(long trans_id) const
	{
		for (size_t i=0; i < items.size(); ++i)
			
			if (CTrans * p = items[i].ptr) 
				if (p->type_id() == trans_id) return int(i);
		return -1;
	}

//#define COMPILE_TIME

	long CTransClip::compile_trans( KSrcDataContext& src_data)
	{
		
		//if (src_data.points_cont.common_points_count(from, to) < 4) return ERR_NO_POINTS;
		check_is_valid(src_data);
		if (!valid) return ERR_TRANSITION_UNDEFINED;
#ifdef COMPILE_TIME
		_TCHAR buff[300] = {0};
		::GetCurrentDirectory ( 256, buff );
		std::wfstream file(::wcscat(buff, L"\\perfomance.txt"), std::ios::app);
		file << L"---------------------\n";
		file << L"\( " << from_ID << L" -> " << to_ID << L" ) ";
		file << L"points count = " << src_data.points_cont.valid_count(from, to) << L"\n";
#endif
		bool ok = false;
		for (iter i = items.begin(); i!= items.end(); ++i)
			if (CTrans *t = i->ptr)
				if (t->is_enabled() && !t->is_ready())
				{
#ifdef COMPILE_TIME
					LARGE_INTEGER time0, time1, freq;
					zero (time0); zero(time1);
					BOOL ok0 = ::QueryPerformanceCounter ( &time0 );
#endif
					ok = ok || t->compile( src_data, this) == ERR_NO_ERROR;
					set_not_ready ();

#ifdef COMPILE_TIME
					BOOL ok1 = ::QueryPerformanceCounter ( &time1 );
					
					
					file << L"\t" << t->trans_name() << L": ";

					if (ok0 && ok1 && ::QueryPerformanceFrequency (&freq) )
					{
						file << double (time1.QuadPart - time0.QuadPart) *1000 / freq.QuadPart << L" ms\n";
					}
					else
						file << L" counter error \n";
#endif	
				}

#ifdef COMPILE_TIME
		file.close();
#endif

		return ok ? ERR_NO_ERROR : ERR_NO_POINTS;
	}

	static long sign(long x) 
	{
		if (x>0) return +1;
		if (x<0) return -1;
		return 0;
	}
//--------------------------------------------------------------------------------------------
	///универсальный обходчик по периметру
	/**
		ф-ция virtual KPOINT point (long x) const = 0 возвращает напрвление обхода. 
		необходимо заместить в потомке
	
		см. trans::CTransClip::compile_vis_poly
		см. vertical_trackman & horizontal_trackman
	*/
	class the_sides_trackman 
	{
		CTransClip * tc;

		long begin;
		long end;

		unsigned NSteps;
	//-----------------------------------
		bool ok(long _err) const { return _err == ERR_NO_ERROR; }
		bool is_err_changed (long err1, long err2) const { return ok(err1) != ok(err2);}

		void find_it( long _begin, long _end)
		{
			
			long dx = sign(_end - _begin);
			if (!dx) return;

			KPOINT last_p;

			long last_x = _begin;
			long curr_err = get_res ( last_x, &last_p);

			for (long x = _begin + dx; x != _end + dx; x += dx)
			{
				KPOINT p;
				long _err = get_res(x, &p);
				if (is_err_changed(_err, curr_err)) 
				{
					on_found( last_p, p, last_x, x, _err);
					curr_err = _err;
				}
				last_x = x;
				last_p = p;
			}
		}
	/// Вызывается, когда найден горизонт
		void on_found( KPOINT last_p, KPOINT p, long last_x, long x, long _err)
		{
			if (ok(_err)) 
			{
				add_point_to_poly(p);
				push_enter_point(x);
			}
			else
			{
				add_point_to_poly(last_p);
				push_leave_point(last_x);
			}
		}
	//-----------------------------------
		
	//-----------------------------------
		/// получить результат на каком-то шаге
		long get_res(long x, KPOINT * pres) const 
		{ 
			KPOINT p = point(x);
			return tc->get_res( p, pres);
		}

		void push_enter_point	(long x)	{tc->horiz.push_enter_point(point(x));}
		void push_leave_point	(long x)	{tc->horiz.push_leave_point(point(x));}
		void add_point_to_poly	(KPOINT p)	{tc->vis_poly.push(p);}
		void add_point_to_source_poly	(KPOINT p)	{tc->source_vis_poly.push(p);}

	protected:
		virtual KPOINT point (long x) const = 0;

		the_sides_trackman(CTransClip * _tc, long _begin, long _end, unsigned _NSteps) 
			:  tc(_tc), begin(_begin), end (_end), NSteps(_NSteps)
		{
			//do_it();
		}
	public:
		void do_it()
		{
//			long dx = sign (end - begin);
			double _step = double (end - begin) / NSteps;
			long x = begin;
			KPOINT res, p = point(x);
			long curr_err = tc->get_res( p, &res );

			if (ok(curr_err))
			{
				add_point_to_poly(res);
				add_point_to_source_poly(p);
			}

			for (unsigned n=1; n < NSteps; ++n)
			{
				long last_x = x;
				x = long (_step*n + begin + 0.5);
				p = point(x);
				long _err = tc->get_res( p, &res );
				
				if (is_err_changed (_err, curr_err)) find_it( last_x, x);
				if (ok(_err))
				{
					add_point_to_poly(res);
					add_point_to_source_poly(p);
				}
			}
		}
	};
//---------------------------
	/// класс для обхода вертикальных сторон мастера по периметру
	/// см. trans::CTransClip::compile_vis_poly
	class vertical_trackman :public the_sides_trackman//обходчик сторон
	{
		long the_x;
	protected:
		virtual KPOINT point (long x) const { return KPOINT (the_x, x);}

	public:
		vertical_trackman(CTransClip * _tc, long _begin, long _end, long _the_x, unsigned _NSteps) 
			: the_sides_trackman( _tc, _begin, _end, _NSteps), the_x(_the_x)
		{
		}
	};
//---------------------------
	/// класс для обхода горизонтальных сторон мастера по периметру
	/// см. trans::CTransClip::compile_vis_poly
	class horizontal_trackman :public the_sides_trackman//обходчик сторон
	{
		long the_y;
	protected:
		virtual KPOINT point (long x) const { return KPOINT (x ,the_y);}

	public:
		horizontal_trackman(CTransClip * _tc, long _begin, long _end, long _the_y, unsigned _NSteps) 
			: the_sides_trackman( _tc, _begin, _end, _NSteps), the_y(_the_y)
		{
		}
	};

//--------------------------------------------------------------------------------------------
	/**
		копирует опорные точки, учавствующие в преобразовании в CPoly
	*/
	void CTransClip::copy_points_to_poly(const KSrcDataContext& src_data) 
	{
		vis_poly.clear();
		source_vis_poly.clear();

		int n = src_data.points_cont.PointsCount();
		for ( int i = 0; i < n; ++i)
		{
			if (src_data.points_cont.is_point_common ( from, to, i) )
			{
				KPOINT p_from = src_data.points_cont.get_point ( from, i );
				KPOINT p_to = src_data.points_cont.get_point ( to, i );
				KPOINT dummy;
				if (get_res( p_from, &dummy) !=  ERR_NO_ERROR) continue;
				vis_poly.push(p_to);
				source_vis_poly.push ( p_from );
			}
		}
	}


	struct HorizFinder
	{
		CTransClip& trans;
		const int steps_count;
		HorizFinder(CTransClip& cp, int NSteps) : trans(cp), steps_count(NSteps) {}

		template<class GetX>
		void Go(long start, long end, long y, GetX X)
		{
			KPOINT ps = make_point(start, y, X), pm;
			bool inf = trans.get_res ( ps, &pm) != ERR_NO_ERROR;

			GoEx( ps, end, inf, X);
		}
	private:
		void PutPointPolygon(KPOINT ps, KPOINT pm)
		{
			trans.vis_poly.add (pm);
			trans.source_vis_poly.add (ps);
		}


		template<class GetX>
		static KPOINT make_point(long x, long y, GetX X)
		{
			typename getOther<GetX >::type Y;
			KPOINT p; X(p) = x; Y(p) = y; return p;
		}
		// предполагает, что prev_ps уже обработана, и, если было нужно, занесена в полигон
		template <class GetX>
		void GoEx(KPOINT prev_ps, long end, bool is_inf, GetX X)
		{
			double dx = double (end - X(prev_ps)) / steps_count;

			typename getOther<GetX>::type Y;
			long y = Y(prev_ps);
			long x0 = X(prev_ps);
			// обходим сторону
			for (int n = 1; n <= steps_count; ++n)
			{
				long x = long (x0+n*dx+0.5);
				KPOINT ps = make_point ( x, y, X), pm;
				if (ps == prev_ps) continue;
				bool is_inf_now = trans.get_res( ps, &pm) != ERR_NO_ERROR;

				if (!is_inf_now)
					PutPointPolygon( ps, pm);
				
				if (is_inf != is_inf_now)
				{
					long x0 = X(prev_ps), x1 = X(ps);
					if (abs (x1-x0) > 1)
						GoEx( prev_ps,  x1, is_inf_now, X);
					else
					{
						if (is_inf_now)
							trans.horiz.push_leave_point(prev_ps);
						else
							trans.horiz.push_enter_point(ps);
					}
				}
				
				if (x == end) return;
				prev_ps = ps;	//prev_pm = pm;
			}
		}
	};

	// скомпилировать vis_poly & source_vis_poly
	void CTransClip::compile_vis_poly(struct KSrcDataContext & src_data)
	{
		if (!valid)		return; 
		if (src_data.is_map(from)) return; // только с мастера на карту
		
		vis_poly.clear();
		source_vis_poly.clear();
		horiz.clear();

		if (!src_data.vispoly_enabled) return;

		long w_from = src_data.get_master(from)->width();
		long h_from = src_data.get_master(from)->height();

		long w_to = src_data.get_master(to)->width();
		long h_to = src_data.get_master(to)->height();
		// обходим периметр мастера с шагом NSteps

		const unsigned NSteps = 20;

		HorizFinder hf ( *this, NSteps );

		long x0=0,y0=0,x1=w_from-1,y1=h_from-1;
/*
		hf.Go( x0, x1, y0,	getX<KPOINT>() );
		hf.Go( y0, y1, x1,  getY<KPOINT>() );
		hf.Go( x1, x0, y1,	getX<KPOINT>() );
		hf.Go( y1, y0, x0,	getY<KPOINT>() );
*/	
		horizontal_trackman		h1(this,	0,				w_from,		0,			NSteps);	
		h1.do_it();
		
		vertical_trackman		v1(this,	0,				h_from,		w_from - 1, NSteps);	
		v1.do_it();

		horizontal_trackman		h2(this,	w_from - 1,		-1,			h_from - 1, NSteps);	
		h2.do_it();

		vertical_trackman		v2(this,	h_from - 1,		-1,			0,			NSteps);	
		v2.do_it();
//*/
		horiz.finalize();
		vis_poly.cut_rect(KRECT( w_to, h_to));
		if (vis_poly.points_count() < 3)
		{
			copy_points_to_poly (src_data); 
			vis_poly.build_convex();
			source_vis_poly.build_convex();
		}
	}
	
	struct empty_vect
	{
		struct empty_obj
		{
			void ser(ser::CElement& /*e*/) {}
		};
		void ser(ser::CElement& e)
		{
			std::vector <empty_obj> v;
			e.ser_vect( _T("empty_obj"), v);
		}
	};
//------------------------------------------------------------------------------------------	


	extern ThreadSafeGlobal<CreateInfo> g_create_data;
	
	//////////////////////////////////////////////
	void CTransClip::ser(ser::CElement& e)
	{
		e.attr( _T("Label"), lb_str);
		e.attr( _T("index_TO"),	to);
		e.attr( _T("index_FROM"), from);
		e.attr( _T("index_TO"),	to);
		e.ser_obj(_T("vis_poly"), vis_poly);
	
		CreateInfo ci = g_create_data.get();
		ci.from = from;
		ci.to = to;
		g_create_data.write(ci);

		e.ser_vect(_T("Transition"), items);

	}
	//////////////////////////////////////////////
	void CTransClip::set_indexes( int _from, int _to, const _TCHAR * lb_text)
	{
		from = _from; to = _to; lb_str = std::tstring (lb_text);
	}
	
	void CTransClip::old_load(CBufferInput& /*Input*/) {}
		
	void CTransClip::clear()
	{
		for (iter i = items.begin(); i != items.end(); ++i) 
			i->release_trans();
			
		if (!items.empty()) items.clear();
		vis_poly.clear();

		set_not_ready ();
	}

	CTransClip::~CTransClip() 
	{ 
		clear(); 
	}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
	long CTransClip::get_res ( KPOINT Src, KPOINT * pRes) const
	{

		if (!valid) return ERR_TRANSITION_UNDEFINED;
		bool trans_defined = false;

		for (r_iterc ri = items.rbegin(); ri != items.rend(); ++ri)
			if (CTrans *t = ri->ptr)
				if (t->is_enabled())
				{
					KPOINT rp;
					trans_defined = true;

					if (t->get_res ( Src, &rp)) 
					{ 
						*pRes = rp; 
						last_tr = t->trans_name();
						return ERR_NO_ERROR;
					}
				}
		last_tr = _T(" NO TRANS ");
		return (trans_defined) ? (ERR_TRANSITION_INFINITY) : (ERR_TRANSITION_UNDEFINED);
	}
////////////////////////////////////////////////////////////////////////////
	void CTransClip::calc_quality( KSrcDataContext& src_data)
	{
		_quality = 0;
		double d = 0;
		unsigned n = 0;
		const CPointDump * pd = &src_data.points_cont;
		for (unsigned i = 0; pd->find_valid (&i, from, to); ++i)
		{
			KPOINT src = pd->get_point( from, i);
			KPOINT res;
			if (get_res( src, &res) == ERR_NO_ERROR)
			{
				d += res.sqr_distance ( pd->get_point( to, i) );
				n++;
			}	
		}

		if (!n) return;

		_quality = 100.0 / ( 1 + 0.1 * sqrt(d) / n );
	}

	CTrans * CTransClip::get_trans(const _TCHAR * trans_name)
	{
		const std::tstring tn (trans_name);
		for (iter i = items.begin(); i != items.end(); ++i)
		{
			if (CTrans * t = i->ptr)
				if (std::tstring(t->trans_name()) == tn) return t;
		}
		return 0;
	}



	CTransClip::CTransClip() 
			:dummy_str (sys::get_res_string(IDS_OPTIONS_CAPTION1)),
			 last_tr	( _T(" ")),
			 _quality	(0),
			 from_ID	(-1),
			 to_ID		(-1),
			 valid		(true)

	{
		items.reserve(5);
	}

	void CTransClip::check_is_valid( const KSrcDataContext& src_data)
	{
		if ((from_ID < 0) && (to_ID < 0))
		{
			// иницилизация
			from_ID = src_data.master_id(from);			
			to_ID = src_data.master_id(to);
			valid = true;
		}
		else
		{
			valid = (from_ID == src_data.master_id(from)) && (to_ID == src_data.master_id(to));
		}
					
	}

	long CTransClip::compile( KSrcDataContext& src_data)
	{
		check_is_valid(src_data);
		
		if (!valid)
		{
			vis_poly.clear();
			horiz.clear();
			return ERR_TRANSITION_UNDEFINED;
		}
			
			long the_res = compile_trans( src_data);
			if (the_res)
			{
				vis_poly.clear();
				horiz.clear(); 
				return the_res;
			}
			
			compile_vis_poly(src_data);
			
			calc_quality(src_data);
			
			return 0;
	}

	
	bool CTransClip::is_this(int _from, int _to) const 
	{ 
		return (from == _from) && (to == _to); 
	}

	void CTransClip::get_vis_poly_buff(const KPOINT ** pptr, size_t * psize) const
	{
		* pptr = vis_poly.data();
		*psize = vis_poly.points_count();
	}


	void CTransClip::bind_opts	(opts::KDialog * d, opts::Itm * i)
	{
		opts::Itm * i2 = d->text ( i, IDC_STATIC_ROOT, &dummy_str);
		for (iter it = items.begin(); it != items.end(); ++it)
			
			it->bind_opts( d, i2);
			
	}

	void CTransClip::reset()
	{
		set_not_ready();
		for (iter it = items.begin(); it != items.end(); ++it)
		
			if (CTrans * t = it->ptr) 
								t->reset();
	}

}