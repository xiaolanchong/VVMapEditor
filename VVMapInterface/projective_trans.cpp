
#include "stdafx.h"
#include "projective_trans.h"
#include "solver.h"

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

namespace trans
{
	
	const _TCHAR * proj_str = _T("projective");


	long CProjectiveTrans::compile( KSrcDataContext& src_data, CTransClip * the_clip ) 
	{
		COverlapInfo OverlapCalc;

		OverlapCalc.bas = Opts.bas;
		OverlapCalc.maxfunc = (Opts.UseSuper) ? (Opts.maxbas) : (0);
		OverlapCalc.ppf = Opts.ppf;

		int NumOfPoints = src_data.points_cont.PointsCount();
		
		int ifrom = the_clip->ifrom();
		int ito = the_clip->ito();

		int N=0;

		for(int k=0; k<NumOfPoints; k++)
		{
			KPOINT p1 = src_data.points_cont.get_point( ifrom, k);
			KPOINT p2 = src_data.points_cont.get_point( ito, k);
			if (!(p1.valid() && p2.valid())) continue;
				
			OverlapCalc.P1[N] = CP( p1.x, p1.y);
			OverlapCalc.P2[N] = CP( p2.x, p2.y);
			N++;	
			
		}	//¬ыбрали все точки дл€ этой пары
				
		masters::CFrame2D_pic * f1 = src_data.get_master(ifrom);
		masters::CFrame2D_pic * f2 = src_data.get_master(ito);
		
	//проективное
		if(N>=4)
		{
			OverlapCalc.hl = f1->height();		OverlapCalc.hr = f2->height();
			OverlapCalc.wl = f1->width();		OverlapCalc.wr = f2->width();

			OverlapCalc.N=N;
			OverlapCalc.CalcTrans(st);

			OverlapCalc.CorrectLSign(&st);

			return ERR_NO_ERROR;
		}
		else 
			return ERR_NO_POINTS;
	}
		//////////////////////////////////////////////
	bool CProjectiveTrans::get_res ( KPOINT Src, KPOINT * pRes) const 
	{ 
		double L = 333;
		long res = st.Trans(Src, * pRes, L);
		
		if (L<0) 
				return false;
		
		return (res == 0);
	}
	//////////////////////////////////////////////
	void CProjectiveTrans::ser(ser::CElement& e) 
	{
		e.attr (_T("Enabled"), enabled);
		e.attr (_T("Using_Super"), Opts.UseSuper);
		e.attr (_T("Basis"), Opts.bas);
		e.attr (_T("Max_basis"), Opts.maxbas);
		e.attr (_T("PPF"), Opts.ppf);
		
		e.attr( _T("Basis"), st.Basis);
		e.attr( _T("Size"), st.Size);
		e.attr( _T("KnownCoeffs"), st.KnownCoeffs);

		e.ser_obj( _T("_super_transitions"), st);

		e.attr( _T("NumCo_0_"), st.NumCo[0]);
		e.attr( _T("NumCo_1_"), st.NumCo[1]);
		e.attr( _T("NumCo_2_"), st.NumCo[2]);
		e.attr( _T("NumCo_3_"), st.NumCo[3]);

		e.attr( _T("src_width"), st.w);
		e.attr( _T("src_height"), st.h);

		//e.ser_array( _T("X_co"), &(st.X_co), &(st.Size) );
		//e.ser_array( _T("Y_co"), &(st.Y_co), &(st.Size) );

	}

	
	bool CProjectiveTrans::base_num_checker::check ( long& val, std::tstring * msg_str)
	{
		if ((val<0) || (val>3))
		{
			*msg_str = sys::get_res_string (IDS_WRONG_BASIS);
			return false;
		}
		return true;
	}
	////////////////////////////////////////////////////////////////
	bool CProjectiveTrans::func_num_checker::check ( long& val, std::tstring * msg_str)
	{
		if ( (val<0) || (val>20) )
		{
			*msg_str = sys::get_res_string (IDS_MAX_NUM_FUNCTIONS);
			return false;
		}
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////















