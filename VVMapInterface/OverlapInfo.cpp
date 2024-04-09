// OverlapInfo.cpp: implementation of the COverlapInfo class.
//
//////////////////////////////////////////////////////////////////////

//#include <windows.h>
#include "stdafx.h"
#include "old_classes.h"
#include "solver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace trans
{

	COverlapInfo::COverlapInfo()
	{
		N=0;
		Ready=FALSE;

		bas=3;
		maxfunc=11;
		ppf=5;
	}

	COverlapInfo::~COverlapInfo()
	{
	}
/*
	double COverlapInfo::CalcTrans()
	{
		int i,j=0;
		
		Calc.N=N;
		for(i=0;i<N;i++){
			Calc.P1[i]=P1[i];
			Calc.P2[i]=P2[i];
		}
		if(N<4)return -1;

		if(Calc.Calculate())return -1;
		for(i=0;i<3;i++)for(j=0;j<3;j++)B21[i][j]=Calc.Result[i][j];
		for(i=0;i<N;i++){
			Calc.P2[i]=P1[i];
			Calc.P1[i]=P2[i];
		}
		if(Calc.Calculate())return -1;
		for(i=0;i<3;i++)for(j=0;j<3;j++)B12[i][j]=Calc.Result[i][j];
		
		return 0;
	}
*/
	POINT COverlapInfo::Trans(POINT P, BOOL Dir)
	{
		double L;
		if(Dir){
			L=(long)(B21[2][0]*P.x+B21[2][1]*P.y+B21[2][2]);
			return CP((long)((B21[0][0]*P.x+B21[0][1]*P.y+B21[0][2])/L),(long)((B21[1][0]*P.x+B21[1][1]*P.y+B21[1][2])/L));
		}else{
			L=(long)(B12[2][0]*P.x+B12[2][1]*P.y+B12[2][2]);
			return CP((long)((B12[0][0]*P.x+B12[0][1]*P.y+B12[0][2])/L),(long)((B12[1][0]*P.x+B12[1][1]*P.y+B12[1][2])/L));
		}
	}

	POINT CP(long x,long y){
		POINT r;
		r.x=x;r.y=y;
		return r;
	}

	double COverlapInfo::AvgDiscr()
	{
		if(!Ready)return 100;
		long i;
		POINT P;
		double d=0;
		for(i=0;i<N;i++){
			P=Trans(P1[i]);
			d+=std::max(abs(P.x-P2[i].x),abs(P.y-P2[i].y));
		}
		d/=N;
		return d;
	}

	long COverlapInfo::CalcTrans(CSuperTransition &T)
	{
		//const long B=3;		//Базис
		T.SetBas(bas);
		T.Size = std::min(maxfunc,  T.GetNumCo(bas));
		
	
		T.h = hl;	T.w = wl;
		
		if(T.Calculate(N,P1,P2,ppf)) return -1;
		CorrectLSign( &T);

		return 0;
	}


	void COverlapInfo::CorrectLSign(CSuperTransition * T)
	{
			
		
		double L = 3333;
				
		int nPos = 0; 	int nNeg = 0; 
						
		///------------------------------
		for (int i=0; i<N; i++) 
		{
			KPOINT p(P1[i]);
			KPOINT dummy;
			
					
			T->Trans(p, dummy, L);
			

			if (L > 0)	nPos++; 
			else		nNeg++;			
		} 
				

		if (nPos < nNeg)
			for (int ix=0; ix < 3; ix++)
			for (int iy=0; iy < 3; iy++)
				T->BaseCalc.Result[ix][iy] = - T->BaseCalc.Result[ix][iy];

			

	}
		
}
	
