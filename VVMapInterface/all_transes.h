
#ifndef _ALLTRANSES_INCLUdED
#define _ALLTRANSES_INCLUdED

#include "trans.h"
#include "kpoint.h"

#include "opts_base.h"
#include "res/resource.h"

namespace trans
{
	
	/// класс, отвечающий за оценку прогресса компиляции и остановку прогресса компиляции  по требованию пользователя
	class CProgress
	{
		/// сколько процентов отпущено нак компиляцию, а сколько на составление карты видимости
		enum {	compile_part = 40, vismap_part = 60	}; 	

		long& rvalue;
		long& rstop;
		
	public:

		/// конструктор
		/**
			rval - переменная-приёмник для прогресса от "0" до "100"
			_rstop - сслыка на переменную-флаг, если rstop != 0 процесс компиляции должен быть остановлен
		*/
		CProgress (long& rval, long& _rstop) :rvalue (rval), rstop(_rstop) {}

		/// установит прогресс
		/**
			Параметры:
				part -	0 во время компиляции
						1 - во время составления катры видимости 

				progress - значение прогресса от 0 до 1
		*/
		void set_progress(int part, double progress)
		{
			const int parts[] = { compile_part, vismap_part};
			int s = sizeof(parts)/sizeof(int);
			if (part >= s || part < 0) return;

			long p = 0;
			for (int i=0; i<part; ++i) p += parts[i];
			rvalue = (long) (p + progress * parts[part]);
		}

		long value() const { return rvalue;}
		bool stop() const { return rstop != 0;}
	};


	struct two_indexes
	{
		two_indexes() :i1(-1), i2(-1) {}
		two_indexes(int _i1, int _i2) :i1(_i1), i2(_i2) {}
		int i1; int i2;
		bool ready() const { return (i2 >= 0) && (i1 >= 0);}
	};

//--------------------------------------------------
	/// класс в котором содержатся все преобразования 
	class CTransCont
	{
		typedef std::vector<CTransClipPtr> vect;
		vect items; /// массив указателей на преобразования
		
		typedef vect::iterator iter;
		typedef vect::const_iterator iterc;

		KSrcDataContext& src_data; /// ссылка на "исходные" данные (информация о мастерах и проч.)

		/// массив массивов пар индексов
		std::vector < std::vector<two_indexes> > routes; 

		long _get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
		long __get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
//------------------------------------------------		
		void log_text (long getxy_res, int ifrom, KPOINT Src, int ito, KPOINT * pRes) const; 
		/// (выключено) нужна для для вывода LogWindow

		mutable const _TCHAR * last_trans; /// название преобразования после работы log_text(выключено)
	//	mutable CLogWin * lw;
		bool make_log;

		/// создает LogWindow	(выключено)
		void create_log_win() const;
		/// убивает LogWindow	(выключено)
		void delete_log_win() const;
//------------------------------------------------
		
		long compile_trans( CProgress * prog);	/// скомпилировать все пребразования (но не карты видимости)
		void compile_vis_map1( iter i);			/// скомпилировать одну карту видимости
		long compile_vis_maps( CProgress * prog);	/// скомпилировать все карты видимости

		/// заполняет массив routes. вызывается на стадии компиляции
		void fill_routes();

		void set_all_ready();

		bool iz_changed( long master_index) const;

		bool _compile_all_next_time;
		void  calc_quality();
				
//--------------------------------------------------
		/// возвращает название мастера с номером master_i.
		std::tstring i2name(int master_i) const;
		
		/// выдаёт строку для преобразования с индексами {_from, _to} каким оно будет представлено в списке диалога настроек 
		std::tstring gen_trans_lb_label( int _from, int _to) const; 
		
		
	public:

		CTransCont(KSrcDataContext& _data);
		~CTransCont();

		/// очистить 
		void clear();
//--------------------------------------------------

		/// ищет преобразования из мастера ifrom в мастер ito, если не находит возвращает 0
		CTransClip * find(int ifrom, int ito);
		
		/// ищет преобразования из мастера ifrom в мастер ito, если не находит возвращает 0
		const CTransClip * find(int ifrom, int ito) const;
		
//--------------------------------------------------
		/// gjr
		long get_xy_pure(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
		long get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
//--------------------------------------------------
//--------------------------------------------------
		long compile( CProgress * prog);
		void shadow_compile_trans();

		void reserve_transes(bool reset_all = false);
		void compile_all_next_time() { _compile_all_next_time = true;}
//--------------------------------------------------		
		void bind_opts(opts::KDialog * d, opts::Itm * i);
//--------------------------------------------------
		virtual void ser(ser::CElement& e);
//--------------------------------------------------
		void CTransCont::load_old_opts( CBufferInput& Input); 
	};

	
}

#endif