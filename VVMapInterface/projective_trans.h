#ifndef __PROJECTIVE_TRANS_H__
#define __PROJECTIVE_TRANS_H__

#include "trans.h"
#include "old_classes.h"

#include "opts_base.h"
#include "res/resource.h"

namespace trans
{	
	
	enum
	{
		projective_id = 1000,
	};

	extern const _TCHAR * proj_str;


	class CProjectiveTrans :public CTrans
	{		
	//	COverlapInfo OverlapCalc;

		bool enabled;
		CSuperTransition st;
		CVVMOptions Opts;
		void fill_wh();
	private:
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
		struct base_num_checker : opts::CheckVal<long>
		{
			virtual bool check ( long& val, std::tstring * msg_str);
		};
		////////////////////////////////////////////////////////////////
		struct func_num_checker : opts::CheckVal<long>
		{
			virtual bool check ( long& val, std::tstring * msg_str);
		};

		base_num_checker bn_ch;
		func_num_checker fn_ch;
	public:
		//////////////////////////////////////////////

		CProjectiveTrans(const KSrcDataContext &dc, long _from, long _to, bool _enabled) 
			:CTrans (dc, _from, _to), enabled(_enabled) {}
		virtual long compile( KSrcDataContext& src_data, CTransClip * the_clip ) ;
		//////////////////////////////////////////////
		virtual bool get_res ( KPOINT Src, KPOINT * pRes) const ;

		virtual bool is_ready()		const { return st.Ready != 0;}
		virtual bool is_enabled()	const { return enabled;}
		virtual void reset() { st.Ready = false;}
		//////////////////////////////////////////////
		virtual void ser(ser::CElement& e) ;
		virtual long type_id() const { return projective_id;}
		virtual const _TCHAR * trans_name() const { return proj_str;}

		virtual void bind_opts	(opts::KDialog * d, opts::Itm * i)
		{
			opts::Itm * i1 = d->bind( i, IDC_PROJ_ENABLED, &enabled);
			opts::Itm * i2 = d->bind( i1, IDC_USESUPER, &Opts.UseSuper);
			
			d->bind( i2,	IDC_EDIT_BASIS,			&Opts.bas,		&bn_ch);
			d->bind( i2,	IDC_EDIT_FUNCTIONS,		&Opts.maxbas,	&fn_ch);
			d->bind( i2,	IDC_EDIT_PPF,			&Opts.ppf);
		}
		virtual ~CProjectiveTrans() 
		{
		}

		void set_opts_old(const CVVMOptions * pOpts) { Opts = *pOpts;}

		virtual CTrans * create_copy() const { return new CProjectiveTrans(*this);}
	};
/*
	class old_loader
	{
	public:
		void old_load (CTrans
	};-*/
}

#endif