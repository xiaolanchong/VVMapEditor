// TransitionCalculator.cpp: implementation of the CTransitionCalculator class.
//
//////////////////////////////////////////////////////////////////////

//#include "TransitionCalculator.h"
#include "stdafx.h"
#include "old_classes.h"
#include "solver.h"


namespace trans
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	void CTransitionCalculator::ser(ser::CElement& e)
	{
		e.attr (_T("Result_0_0"), Result[0][0]);
		e.attr (_T("Result_0_1"), Result[0][1]);
		e.attr (_T("Result_0_2"), Result[0][2]);
		e.attr (_T("Result_1_0"), Result[1][0]);
		e.attr (_T("Result_1_1"), Result[1][1]);
		e.attr (_T("Result_1_2"), Result[1][2]);
		e.attr (_T("Result_2_0"), Result[2][0]);
		e.attr (_T("Result_2_1"), Result[2][1]);
		e.attr (_T("Result_2_2"), Result[2][2]);
	}

	CTransitionCalculator::CTransitionCalculator() :N(0)
	{
	}

	CTransitionCalculator::~CTransitionCalculator()
	{
	}

	long CTransitionCalculator::Calculate()
	{
		if(N<4)return 1;

		long n[4],no[4]={0,1,2,3};

		double D,Dmin=1e10;
#ifdef COMPILE_TIME
		LARGE_INTEGER time0, time1, freq;
		BOOL ok0 = ::QueryPerformanceCounter ( &time0);
#endif

		for(n[0]=0;n[0]<N;n[0]++)
			for(n[1]=n[0]+1;n[1]<N;n[1]++)
				for(n[2]=n[1]+1;n[2]<N;n[2]++)
					for(n[3]=n[2]+1;n[3]<N;n[3]++)
					{
						D=Calculate4(n);
						if(D<Dmin)
						{
							Dmin=D;
							CopyMemory(no,n,sizeof(n));
						}	
					}
#ifdef COMPILE_TIME
		BOOL ok1 = ::QueryPerformanceCounter ( &time1) && ::QueryPerformanceFrequency (&freq);
#endif
		Calculate4(no);
		CopyMemory(Result,Result1,9*sizeof(double));

		double dif,newdif=Difference();
		if(newdif<N*25){
			return 0;
		}
		double d,dsum;
		long m=0,k=0,l=0;

		double STEP=1;
		do{
			dif=newdif;dsum=0;
			Result1[0][0]-=STEP*dif/(d=Delta(0,0)); dsum+=(d>0)?d:-d;
			Result1[0][1]-=STEP*dif/(d=Delta(0,1)); dsum+=(d>0)?d:-d;
			Result1[0][2]-=STEP*dif/(d=Delta(0,2)); dsum+=(d>0)?d:-d;
			Result1[1][0]-=STEP*dif/(d=Delta(1,0)); dsum+=(d>0)?d:-d;
			Result1[1][1]-=STEP*dif/(d=Delta(1,1)); dsum+=(d>0)?d:-d;
			Result1[1][2]-=STEP*dif/(d=Delta(1,2)); dsum+=(d>0)?d:-d;
			Result1[2][0]-=STEP*dif/(d=Delta(2,0)); dsum+=(d>0)?d:-d;
			Result1[2][1]-=STEP*dif/(d=Delta(2,1)); dsum+=(d>0)?d:-d;
			newdif=Difference();
			if(dsum*STEP<0.0001) break;
			if(newdif>dif){
				newdif=dif;
				STEP/=10;k++;
				if(k>6) break;
				CopyMemory(Result1,Result,9*sizeof(double));
			}else{
				k=0;l++;
				if(l>5){
					l=0;STEP*=2;
				}
				CopyMemory(Result,Result1,9*sizeof(double));
			}
		}while(m++<1000);		//Максимальное число итераций ограничим

#ifdef COMPILE_TIME
		std::wfstream file (L"d:\\iteration_count.txt", std::ios::app);
		file << L"m = " << m << std::endl;
		file << L"time = " << double (time1.QuadPart - time0.QuadPart) *1000 / freq.QuadPart  << L" ms \n";
		file.close();
#endif
		return 0;
	}

	double CTransitionCalculator::Difference()
	{
		long i;
		double sum=0,L;
		for(i=0;i<N;i++){
			L=(Result1[2][0]*P1[i].x+Result1[2][1]*P1[i].y+Result1[2][2]);
			if(0==L)return 99999.;
			sum+=
				sq(P2[i].x-(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L)+
				sq(P2[i].y-(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L);
		}
		return sum;
	}

	double CTransitionCalculator::Delta(long a, long b)
	{
		long i;
		double sum=0,L;
		for(i=0;i<N;i++){
			L=(Result1[2][0]*P1[i].x+Result1[2][1]*P1[i].y+Result1[2][2]);
			if(0==L)return 99999.;

			switch(a*3+b){
			case 0: sum+=(P2[i].x-P1[i].x*(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L);break;
			case 1: sum+=(P2[i].x-P1[i].y*(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L);break;
			case 2: sum+=(P2[i].x-(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L);break;

			case 3: sum+=(P2[i].y-P1[i].x*(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L);break;
			case 4: sum+=(P2[i].y-P1[i].y*(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L);break;
			case 5: sum+=(P2[i].y-(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L);break;

			case 6: sum+=(P2[i].x-(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L)*(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/(L*L*P1[i].x)+
						(P2[i].y-(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L)*(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/(L*L*P1[i].x);break;
			case 7: sum+=(P2[i].x-(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/L)*(Result1[0][0]*P1[i].x+Result1[0][1]*P1[i].y+Result1[0][2])/(L*L*P1[i].y)+
						(P2[i].y-(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/L)*(Result1[1][0]*P1[i].x+Result1[1][1]*P1[i].y+Result1[1][2])/(L*L*P1[i].y);break;
			}
		}
		return 2*sum;
	}

	double CTransitionCalculator::Calculate4(long *n)
	{

		double x,y,u,v;
		long i;

		for(i=0;i<4;i++){
			x=P1[n[i]].x;y=P1[n[i]].y;
			u=P2[n[i]].x;v=P2[n[i]].y;
			a[i*16]=-x;
			a[i*16+1]=-y;
			a[i*16+2]=-1;
			a[i*16+3]=0;
			a[i*16+4]=0;
			a[i*16+5]=0;
			a[i*16+6]=x*u;
			a[i*16+7]=y*u;

			a[i*16+8]=0;
			a[i*16+9]=0;
			a[i*16+10]=0;
			a[i*16+11]=-x;
			a[i*16+12]=-y;
			a[i*16+13]=-1;
			a[i*16+14]=x*v;
			a[i*16+15]=y*v;

			b[2*i]=-u;
			b[2*i+1]=-v;
		}
		
		CGaussSolver GS;
		GS.Reset(8,a);

		if(!GS.Solve(X,b)){
			return 1e10;
		}
		Result1[0][0]=X[0];
		Result1[0][1]=X[1];
		Result1[0][2]=X[2];
		Result1[1][0]=X[3];
		Result1[1][1]=X[4];
		Result1[1][2]=X[5];
		Result1[2][0]=X[6];
		Result1[2][1]=X[7];
		Result1[2][2]=1;		//Это типа по первым четырем нашли первое приближение

		return Difference();
	}
}