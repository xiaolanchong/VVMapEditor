
#ifndef _ALLTRANSES_INCLUdED
#define _ALLTRANSES_INCLUdED

#include "trans.h"
#include "kpoint.h"

#include "opts_base.h"
#include "res/resource.h"

namespace trans
{
	
	/// �����, ���������� �� ������ ��������� ���������� � ��������� ��������� ����������  �� ���������� ������������
	class CProgress
	{
		/// ������� ��������� �������� ��� ����������, � ������� �� ����������� ����� ���������
		enum {	compile_part = 40, vismap_part = 60	}; 	

		long& rvalue;
		long& rstop;
		
	public:

		/// �����������
		/**
			rval - ����������-������� ��� ��������� �� "0" �� "100"
			_rstop - ������ �� ����������-����, ���� rstop != 0 ������� ���������� ������ ���� ����������
		*/
		CProgress (long& rval, long& _rstop) :rvalue (rval), rstop(_rstop) {}

		/// ��������� ��������
		/**
			���������:
				part -	0 �� ����� ����������
						1 - �� ����� ����������� ����� ��������� 

				progress - �������� ��������� �� 0 �� 1
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
	/// ����� � ������� ���������� ��� �������������� 
	class CTransCont
	{
		typedef std::vector<CTransClipPtr> vect;
		vect items; /// ������ ���������� �� ��������������
		
		typedef vect::iterator iter;
		typedef vect::const_iterator iterc;

		KSrcDataContext& src_data; /// ������ �� "��������" ������ (���������� � �������� � ����.)

		/// ������ �������� ��� ��������
		std::vector < std::vector<two_indexes> > routes; 

		long _get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
		long __get_xy(int ifrom, KPOINT Src, int ito, KPOINT * pRes) const;
//------------------------------------------------		
		void log_text (long getxy_res, int ifrom, KPOINT Src, int ito, KPOINT * pRes) const; 
		/// (���������) ����� ��� ��� ������ LogWindow

		mutable const _TCHAR * last_trans; /// �������� �������������� ����� ������ log_text(���������)
	//	mutable CLogWin * lw;
		bool make_log;

		/// ������� LogWindow	(���������)
		void create_log_win() const;
		/// ������� LogWindow	(���������)
		void delete_log_win() const;
//------------------------------------------------
		
		long compile_trans( CProgress * prog);	/// �������������� ��� ������������� (�� �� ����� ���������)
		void compile_vis_map1( iter i);			/// �������������� ���� ����� ���������
		long compile_vis_maps( CProgress * prog);	/// �������������� ��� ����� ���������

		/// ��������� ������ routes. ���������� �� ������ ����������
		void fill_routes();

		void set_all_ready();

		bool iz_changed( long master_index) const;

		bool _compile_all_next_time;
		void  calc_quality();
				
//--------------------------------------------------
		/// ���������� �������� ������� � ������� master_i.
		std::tstring i2name(int master_i) const;
		
		/// ����� ������ ��� �������������� � ��������� {_from, _to} ����� ��� ����� ������������ � ������ ������� �������� 
		std::tstring gen_trans_lb_label( int _from, int _to) const; 
		
		
	public:

		CTransCont(KSrcDataContext& _data);
		~CTransCont();

		/// �������� 
		void clear();
//--------------------------------------------------

		/// ���� �������������� �� ������� ifrom � ������ ito, ���� �� ������� ���������� 0
		CTransClip * find(int ifrom, int ito);
		
		/// ���� �������������� �� ������� ifrom � ������ ito, ���� �� ������� ���������� 0
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