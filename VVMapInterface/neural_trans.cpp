#include "stdafx.h"

#include "neural_trans.h"

#include "memleak.h"


#ifdef UNIX_RELEASE
#define _snprintf snprintf
#endif

namespace trans
{
	const _TCHAR * neur_str = _T("neural");


//////////////////////////////////////////////////////////////////////////////////////
	typedef std::vector<long> vect;

	class formula_reader : public vect
	{
		typedef vect::iterator iter;
		typedef vect::const_iterator iterc;
		
		enum { buff_size = 100 };
		char buff[buff_size];
		
	//	const char * text;
		unsigned pos;
		unsigned buff_pos;

		void reset_buff()
		{
			memset (buff, 0, sizeof(buff));		buff_pos = 0;		
		}

		void new_buff()
		{
			if (!buff_pos) return;
			push_back(atol(buff));
			reset_buff();
		}

		void add_to_buff(char c)
		{
			buff[buff_pos] = c;		buff_pos++;
		}
		
	public:
		formula_reader(const char * text) : pos(0), buff_pos(0)
		{
			reset_buff();
			while (char ch = text[pos])
			{
				if (isdigit(ch)) add_to_buff(ch);
				else new_buff();

				pos++;
			}
			new_buff();
		}

		bool test() const 
		{
			///////////////////////////////////////////////////////////////////////
			static const long max_formula[] = {10,50,50,50,10};
			///////////////////////////////////////////////////////////////////////

			size_t f_size = sizeof(max_formula) / sizeof (max_formula[0]);

			if (f_size > size() || empty()) return false;

			for (unsigned i=0; i < size(); ++i)
			{
				long n = operator[](i);
				if ( (n>0) || (n > max_formula[i]) ) return false;
			}

			return true;
		}

		long * ptr() { return &(operator[](0));}
		const long * ptr() const { return &(operator[](0));}
	};
//////////////////////////////////////////////////////////////////////////////////////
	struct learn_pack 
	{
		point_double p_in;
		point_double p_out;
	};

	long CNeuroTrans::compile( KSrcDataContext& src_data, CTransClip * /*the_clip*/ ) 
	{ 
		if (!enabled) return 0;

	//	p_clip = the_clip;

		fill_wh();

	//	src_data.points_cont.set_current_masters( from(), to() );
		
	//	unsigned p_count = src_data.points_cont.curr_count();

		const CPointDump * pd =  &src_data.points_cont;

		unsigned pi			= pd->find_valid ( unsigned(), from(), to() );
		unsigned p_count	= pd->valid_count ( from(), to() );
		
		std::vector <learn_pack> data_to_learn(p_count); 
				
		std::vector <learn_pack>::iterator i = data_to_learn.begin();
		
		while 
			(
			(!pd->end(pi)) && ( i != data_to_learn.end() )	
			)
		{
			i->p_in  = point_from ( pd->get_point( from(), pi) );
			i->p_out = point_to   ( pd->get_point( to(), pi) );
			++i;
			++pi;
		}

		std::string fs = sys::ANSI_str(formula_str.c_str());

		formula_reader f(fs.c_str());

		NNet.Init( f.size() - 1, f.ptr());
		
		if (!data_to_learn.empty())
		{
			double * lp = &data_to_learn[0].p_in.x;
			NNet.SetXLearn( (long) data_to_learn.size(), lp);

			for (int n=0; n < iter_num; n++)
							NNet.Iteration(100);
			ready = true;								
		}
			
		return 1;
	}

	//////////////////////////////////////////////
	zipper::CBuff CNeuroTrans::get_neuro_buff() const
	{
		void * p = 0;
		size_t s = NNet.Save(&p);
		return zipper::CBuff (p, s);
	}

	void CNeuroTrans::init_from_buff(zipper::CBuff buff) const
	{
		void * p = static_cast <void *> (buff.ptr);
		NNet.Load(p);
	}
	std::string CNeuroTrans::gen_fname() const
	{
		const size_t s = 50; char buff[s];
		int f = from(); int t = to();
		_snprintf( buff, s, "misc/neuro/neuro_trans_%i_%i.bin", f, t);
		return std::string (buff);

	}
	//////////////////////////////////////////////
	bool CNeuroTrans::get_res ( KPOINT Src, KPOINT * pRes) const 
	{ 
		if (!enabled) return false;
		if (!ready) return false;
		
		if ( !src_data()->points_cont.is_interpolation_area(from(), to(), Src) )
			return false;

		double xy[2]; 
	
		point_double src_d = point_from(Src);
		xy[0] = src_d.x; 
		xy[1] = src_d.y;

		NNet.GetResult(xy);

		point_double res_d ( NNet.Y(0), NNet.Y(1));
		*pRes = point_to (res_d);
		
		return true;
	}

	bool CNeuroTrans::is_ready() const 
	{ 
		return ready;
	}
	
	bool CNeuroTrans::is_enabled() const 
	{ 
		return enabled;
	}
	//////////////////////////////////////////////
	void CNeuroTrans::ser(ser::CElement& e) 
	{
		e.attr (_T("Enabled"), enabled);
		
		e.attr(_T("FORMULA"),   formula_str);
		e.attr(_T("iter_num"), iter_num);

		std::string fname = gen_fname();
		std::tstring tfname = sys::T_str(fname.c_str());
		e.attr(_T("file_name"),   tfname);	
		//-----------------------------------------
		if (e.is_loading()) 
		{
			try
			{
				zipper::CBuff b;
				e.ser_bin( fname.c_str(), &b);
				init_from_buff(b);
				ready = true;
			}
			catch (ser::no_file& e)
			{
				ready = false;	
				MyTrace ("NeuroTrans::ser caught ser::no_file " + std::string(e.what()));
			}
		}
		else
		{
			if (ready)
			{
				zipper::CBuff b = get_neuro_buff();
				e.ser_bin( fname.c_str(), &b);
				b.release();
			}
		}
		
	}

	long CNeuroTrans::type_id() const 
	{ 
		return neural_id;
	}

	const _TCHAR * CNeuroTrans::trans_name() const 
	{ 
		return neur_str;
	}
	
	////////////////////////////////////////////////////////////////	
	void CNeuroTrans::bind_opts	(opts::KDialog * d, opts::Itm * i) 
	{
		opts::Itm * in = d->bind( i, IDC_USE_NEURAL, &enabled);
		
		d->bind( in, IDC_NEURAL_FORMULA,	&formula_str,	&f_ch);
		d->bind( in, IDC_NEUR_ITER_NUM,		&iter_num,		&i_ch);
	}

	KPOINT CNeuroTrans::point_to ( point_double p) const
	{
		double x = ((p.x + 1)*w_to)/2 ;
		double y = ((p.y + 1)*h_to)/2 ;
		return KPOINT (long (x), long (y));
	}

	KPOINT CNeuroTrans::point_from ( point_double p) const
	{
		double x = ((p.x + 1)*w_from)/2 ;
		double y = ((p.y + 1)*h_from)/2 ;
		return KPOINT (long (x), long (y));
	}

	point_double CNeuroTrans::point_from ( KPOINT p) const
	{
		double x = (2*double(p.x)) / w_from - 1;
		double y = (2*double(p.y)) / h_from - 1;
		return point_double( x, y);
	}

		point_double CNeuroTrans::point_to ( KPOINT p) const
	{
		double x = (2*double(p.x)) / w_to - 1;
		double y = (2*double(p.y)) / h_to - 1;
		return point_double( x, y);
	}

	bool CNeuroTrans::formula_checker::check ( std::tstring& val, std::tstring * msg_str)
	{
		std::string as = sys::ANSI_str(val.c_str());
		formula_reader f (as.c_str());

		if (!f.test())
		{
			*msg_str = sys::get_res_string(IDS_WRONG_FORMULA);
			return false;
		}
		return true;
	}

	bool CNeuroTrans::iternum_checker::check ( long& val, std::tstring * msg_str)
	{
		const long max_val = 100000;
		if (val > max_val)
		{
			const size_t s = 20; _TCHAR b[s]; int mv = max_val;
			_sntprintf( b, s, _T(" %i"), mv);
			*msg_str = sys::get_res_string(IDS_NUM_OF_ITER) + std::tstring(b);
			return false;
		}
		return true;
	}
}