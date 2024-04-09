
#ifndef _TRANS_H_2131323234
#define _TRANS_H_2131323234

//#include "../base/BaseClasses.h"
//#include "../general/primitives.h"
//#include "../Masters/Masters.h"

//#define ERR_INVALID_MASTERS -41

#pragma warning (disable : 4786)
#include "context.h"
#include "kpoint.h"
#include "poly.h"

#include "serializer3.h"
#include "BufferInput.h"

#include "opts_base.h"
#include "res/resource.h"


#include "horiz.h"

namespace trans
{
//	using namespace masters;

	
	class CTransClip;

///////////////////////////////////////////////////////////////////////
	/// базовый класс преобразований
	/**
		различные типы преобразований (проективное, нейронное, триангуляция) наследуются от него
	*/
	class CTrans
	{
		/// ссылка на исходные данные
		const KSrcDataContext& the_src_data; 
		/// индекс мастера откуда преобразование
		long _from; 
		/// индекс мастера куда преобразование
		long _to;	

	//	friend class CTransClip;
	public:
		
		CTrans(const KSrcDataContext& _src_data, long ifrom, long ito) 
			: _from(ifrom), _to(ito), the_src_data(_src_data) {}

		virtual long from() const { return _from;}
		virtual long to() const { return _to;}
		//////////////////////////////////////////////
		virtual const KSrcDataContext * src_data() const { return &the_src_data;}
		//////////////////////////////////////////////
		virtual long compile( KSrcDataContext& src_data, CTransClip * the_clip ) = 0;
		//////////////////////////////////////////////
		virtual bool get_res ( KPOINT Src, KPOINT * pRes) const = 0;

		/// готово ли?
		virtual bool is_ready() const = 0;
		/// включено ли?
		virtual bool is_enabled() const = 0;	
		/// обнулить преобразование
		virtual void reset() = 0;	
		//////////////////////////////////////////////
		/// сериализовать в xml
		virtual void ser(ser::CElement& e) = 0;	
		/// ID преобразования
		virtual long type_id() const = 0;		
		/// имя преобразования
		virtual const _TCHAR * trans_name() const = 0; 
		/// создать копию себя
		virtual CTrans * create_copy() const = 0;		

		/// привязывает диалог настроек
		virtual void bind_opts	(opts::KDialog * d, opts::Itm * i) = 0;
		virtual ~CTrans() {}
	};

///////////////////////////////////////////////////////////////////////

	class CTransClip;

	/// Фабрика классов преобразований
	class CTransFab
	{
		const KSrcDataContext& dc;
		void add_trans(CTransClip * tc, long trans_id);
	public:
		CTransFab (KSrcDataContext& _dc) :dc(_dc) {}
		
		/// создать преобразование. 
		CTrans * create(int trans_type_id, long _from, long _to);
		/// создать преобразование. type_name - тип преобразования
		CTrans * create(const _TCHAR * type_name, long _from, long _to);

		void charge_clip (CTransClip * tc);
	};
//--------------------------------------------------

	/// самопальный smart pointer с поддержкой сериализации и опций

	struct KTransPtr
	{
		CTrans * ptr;

		KTransPtr() : ptr(0) {}
		KTransPtr(CTrans * p) : ptr(p) {}

		void release_trans() 
		{ 
			if (ptr) delete ptr; ptr = 0;
		}
		
		void bind_opts	(opts::KDialog * d, opts::Itm * i)
		{
			ptr->bind_opts( d, i);
		}
		void ser(ser::CElement& e);

		KTransPtr& operator= (const KTransPtr& kp)
		{
			release_trans();
			if (kp.ptr)
			{
				ptr = kp.ptr->create_copy();
			}
			return *this;
		}

	
	};
//-------------------------------------
	class bad_index
	{ 
		int the_index;
	public:
		bad_index(int i) :the_index(i) {}
		int index() const { return the_index; }
	};
//-------------------------------------


	/// Класс-пакет преобразований. Содержит все виды преобразований с одного мастера на другой
	
	class CTransClip 
	{
		std::vector <KTransPtr> items; /// список итемов
//----------------------------------------------------------------------------		
		typedef std::vector <KTransPtr>::iterator iter;
		typedef std::vector <KTransPtr>::const_iterator iterc;
		typedef std::vector <KTransPtr>::reverse_iterator r_iter;
		typedef std::vector <KTransPtr>::const_reverse_iterator r_iterc;
//----------------------------------------------------------------------------
		int from;	int to;
		int from_ID;	
		int to_ID;

		// полигон видимости на мастере (to)
		CPoly vis_poly;
		// полигон видимости на мастере (from)
		CPoly source_vis_poly;

		std::tstring dummy_str;
		
		friend class CTransFab;
		friend class the_sides_trackman;
		friend struct HorizFinder;

		void push(CTrans * tr);
		
		std::tstring lb_str;

		void is_good_i(int i) const 
		{ size_t s = i; if ( s >= items.size()) throw bad_index(i);}

		//long get_point_for_poly(KPOINT p0, KPOINT step_p, long last_res);
		long get_point_for_poly(KPOINT p0, KPOINT step_p, long last_res, bool h);
		//------------------------------------
		bool _ready;
		void check_ready() const;
		void set_not_ready () { _ready = false; _quality = 0; }
		//------------------------------------
		mutable const _TCHAR * last_tr;
		//------------------------------------
		double _quality; // от 0 до 100%
		//------------------------------------
		bool valid;
	
		/// 	копирует опорные точки, учавствующие в преобразовании в CPoly
		void copy_points_to_poly(const KSrcDataContext& src_data);
	public:
		
		/// расчитать качество преобразования. вызывается при компиляции. Значение качества можно получить quality()
		virtual void calc_quality( KSrcDataContext& src_data);
		/// готов
		bool ready() const { return _ready;}
		bool is_valid() const { return valid;}
		void set_ready(bool value = true) { _ready = value; }
		void set_valid(bool value = true) { valid = value; }

		const _TCHAR * last_trans_name() const { return last_tr; };

		CTransClip();
		/*
		CTransClip(const CTransClip& tc);
		CTransClip& operator= (const CTransClip& tc);
		*/
		int get_index (long trans_id) const;

		/// число преобразований
		size_t count () const { return items.size();}
		/// проверяет валидно ли сейчас преобразование. результат работы ф-ции - выставляет флаг valid
		/**
			Отслеживает не изменился ли ID у мастеров, с которыми работает данное преобразование.
			если изменился, преобразование становится невалидным.
		*/
		void check_is_valid( const KSrcDataContext& src_data);

		/// Скомпилировать все. И сами преобразования и полигон видимости
		virtual long compile( KSrcDataContext& src_data);
		/// скопилировать преобразования
		virtual long compile_trans( KSrcDataContext& src_data);
		/// скомпилировать полигон видимости
		virtual void compile_vis_poly( KSrcDataContext& src_data);
		
		
		bool is_this(int _from, int _to) const;
		bool is_inside_vis_poly( KPOINT p) const { return vis_poly.is_inside(p);}
		void get_vis_poly_buff(const KPOINT ** pptr, size_t * psize) const;
		const CPoly& get_vis_poly() const { return vis_poly; }
		const CPoly& get_source_vis_poly() const { return source_vis_poly; }
		

		//////////////////////////////////////////////
		/// Получить результат
		/** 
		3 случая
			0.	ERR_NO_ERROR - преобразование выполнено
		    1.	ERR_TRANSITION_UNDEFINED  - если не валидно или нет действующих преобразований (или они выключены)
			2.	ERR_TRANSITION_INFINITY   - если преобразование в данной точке преобразование 
					не может быть выполнено
		*/

		virtual long get_res ( KPOINT Src, KPOINT * pRes) const;
		//
		
		//////////////////////////////////////////////
		//////////////////////////////////////////////
		virtual void ser(ser::CElement& e);
		//////////////////////////////////////////////
		void set_indexes( int _from, int _to, const _TCHAR * lb_text);
		int ifrom() const { return from; }
		int ito()	const { return to;   }
		//////////////////////////////////////////////
		void old_load(CBufferInput& Input);
		/////////////////////////////////////////////
		/// Label string
		const _TCHAR * LB_string() const { return lb_str.c_str();}

		/// связать с опциями
		void bind_opts	(opts::KDialog * d, opts::Itm * i);
		
		void reset();

		/// получить качество преобразования.
		/**
			вычисляется по формуле quality = 100.0 / ( 1 + 0.1 * sqrt(d) / n );
			Предположим, у нас преобразование с мастера на карту. 
			Пусть на мастере задано множество опорных точек M0, на карте - M1.
			При преобразованиии с мастера на карту M0 переводятся в M.
			M необязательно должна совпадать с M1,
			d - есть сумма квадратов отклонений точек M и соотв. точек из M1
			n - число преобразований.
		*/
		double quality() const { return _quality; }
		
		/// очистить
		void clear();
		virtual ~CTransClip();

		CTrans * get_trans(const _TCHAR * trans_name);
		//-------------------------------------------

		/// "горизонт" проективного пробразования.
		CHorizont horiz;
	};

	/// еще 1 самопальный smart_ptr с поддержкой сериализации
	/**
		CTransCont содержит массив таких указателей
	*/
	struct CTransClipPtr
	{
		CTransClip * ptr;
		CTransClipPtr() :ptr(new CTransClip()), p_refs(new unsigned(1)) {}
		CTransClipPtr(const CTransClipPtr& c) 
		{ 
			ptr = c.ptr; p_refs = c.p_refs; 
			add(); 
		}
		CTransClipPtr& operator= (const CTransClipPtr& c)
		{ 
			release();
			ptr = c.ptr; 	p_refs = c.p_refs; 
			add();
			return *this;
		}

		~CTransClipPtr() { release(); }

		void bind_opts	(opts::KDialog * d, opts::Itm * i)	{ ptr->bind_opts(d,i); }
		void old_load(CBufferInput& Input) { ptr->old_load(Input); }
		void ser(ser::CElement& e) { ptr->ser(e); }
	private:
		

		unsigned * p_refs;
		void add() { (*p_refs)++; }
		void release()
		{
			(*p_refs)--;
			if (!*p_refs)
			{
				delete p_refs;
				delete ptr;
			}
		}

	};
	
//---------------------------------------------------------------------
	/// вспомогательная структура. Используется для создания преобразований при загрузке vme
	struct CreateInfo
	{
		long from, to;
		CTransFab * fab;
		CreateInfo() : from(0), to(0), fab(0) {}
	};

///////////////////////////////////////////////////////////////////////


	
}

#endif