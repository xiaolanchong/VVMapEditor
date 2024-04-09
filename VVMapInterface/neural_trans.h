
#include "trans.h"
#include "../SRS/Sourcelibrary/NeuralNet.h"

namespace trans
{
	enum
	{
		neural_id = 1001,
	};

	extern const _TCHAR * neur_str;


	struct point_double
	{
		double x;
		double y;
		point_double( double _x, double _y) : x(_x), y(_y) {}
		point_double() : x(0.0), y(0.0) {}
	};
	
	class CNeuroTrans :public CTrans
	{	
		KPOINT point_to ( point_double p) const;
		point_double point_to ( KPOINT p) const;
		KPOINT point_from ( point_double p) const;
		point_double point_from ( KPOINT p) const;

		std::tstring formula_str;
		long iter_num;

		long w_from, h_from, w_to, h_to;
		void fill_wh()
		{
			w_from = src_data()->get_master(from())->width();
			h_from = src_data()->get_master(from())->height();
			w_to = src_data()->get_master(to())->width();
			h_to = src_data()->get_master(to())->height();
			
		}
		
		bool ready;
		bool enabled;

		mutable CNeuralNet NNet;

		zipper::CBuff get_neuro_buff() const;
		void init_from_buff(zipper::CBuff buff) const;
		std::string gen_fname() const;

	private:
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		struct iternum_checker : opts::CheckVal<long>
		{
			virtual bool check ( long& val, std::tstring * msg_str);
		};
		////////////////////////////////////////////////////////////////
		struct formula_checker : opts::CheckVal<std::tstring>
		{
			virtual bool check ( std::tstring& val, std::tstring * msg_str);
		};

		iternum_checker i_ch;
		formula_checker f_ch;


	public:

		CNeuroTrans(  const KSrcDataContext &dc, long _from, long _to, bool _enabled) 
			:CTrans (dc, _from, _to), 
			 enabled(_enabled), 
			 ready (false), 
			 formula_str(_T(" 2 3 3 2")),
			 iter_num ( 10000 )
			 
		{
			fill_wh();
		}
		//////////////////////////////////////////////
		virtual long compile( KSrcDataContext& src_data, CTransClip * the_clip ); 
		//////////////////////////////////////////////
		virtual bool get_res ( KPOINT Src, KPOINT * pRes) const;

		virtual bool is_ready() const;
		virtual bool is_enabled() const;
		virtual void reset() { ready = false;}
		//////////////////////////////////////////////
		virtual void ser(ser::CElement& e);
		virtual long type_id() const;
		virtual const _TCHAR * trans_name() const;
		
		virtual void bind_opts	(opts::KDialog * d, opts::Itm * i);

		virtual CTrans * create_copy() const { return new CNeuroTrans(*this);}

	};
}