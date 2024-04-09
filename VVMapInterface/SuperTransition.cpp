// SuperTransition.cpp: implementation of the CSuperTransition class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "old_classes.h"
#include "solver.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace trans
{


	#define _PI 3.14159265358

	void CSuperTransition::ser(ser::CElement& e)
	{
		e.attr( _T("Ready"), Ready);
		
		e.attr( _T("Basis"), Basis);
		e.attr( _T("Size"), Size);
		e.attr( _T("KnownCoeffs"), KnownCoeffs);

		e.attr( _T("NumCo_0_"), NumCo[0]);
		e.attr( _T("NumCo_1_"), NumCo[1]);
		e.attr( _T("NumCo_2_"), NumCo[2]);
		e.attr( _T("NumCo_3_"), NumCo[3]);

		e.attr( _T("src_width"), w);
		e.attr( _T("src_height"), h);

		e.ser_obj(_T("BaseCalc"), BaseCalc);

		e.ser_array( _T("X_co"), &(X_co), &(Size) );
		e.ser_array( _T("Y_co"), &(Y_co), &(Size) );
	}

	CSuperTransition::CSuperTransition() :X_co(0), Y_co(0), Ready(0), KnownCoeffs(0), w(1), h(1)
	//	NumCo({4,7,19,11})
	{//
		NumCo[0] = 4;
		NumCo[1] = 7;
		NumCo[2] = 19;
		NumCo[3] = 11;
	}

	CSuperTransition::~CSuperTransition()
	{
		if (X_co) delete[] X_co;
		if (Y_co) delete[] Y_co;
	}

	double CSuperTransition::Basis_Function(long Bas, long Num, POINT P) const
	{
		switch(Bas){
		case 0:return BF_1(Num,P);
		case 1:return BF_2(Num,P);
		case 2:return BF_3(Num,P);
		case 3:return BF_4(Num,P);
		}
		return 0;
	}

	double CSuperTransition::BF_1(long N, POINT P) const
	{
		double x=(double)P.x;
		double y=(double)P.y;
		switch(N){
		case 0:return sin(x/w*_PI);break;
		case 1:return sin(y/h*_PI);break;
		case 2:return sin(2*x/w*_PI);break;
		case 3:return sin(2*y/h*_PI);break;
		}
		return 0;
	}

	void CSuperTransition::SetBas(long N)
	{
		if(X_co)delete[]X_co;
		if(Y_co)delete[]Y_co;
		Basis = N;
		Size  = NumCo[N];
		long i;
		X_co = new double[NumCo[N]];		for(i=0;i<NumCo[N];i++) X_co[i]=0;
		Y_co = new double[NumCo[N]];		for(i=0;i<NumCo[N];i++) Y_co[i]=0;
		Ready=0;
	}

	long CSuperTransition::Calculate(long NP, POINT *P1, POINT *P2,double pff)
	{
		if(NP<4)return 1;
		KnownCoeffs=Size;//NumCo[Basis];
		
		if(NP<Size*pff)
		{
			KnownCoeffs=(long)(NP/pff);
		}
		long NumEq=2*NP;
		long NumC=2*KnownCoeffs;//2*Size;

		BaseCalc.N=NP;
		CopyMemory(BaseCalc.P1,P1,sizeof(POINT)*NP);
		CopyMemory(BaseCalc.P2,P2,sizeof(POINT)*NP);

		if(BaseCalc.Calculate())return 3;	//Вычислим основу - проективное преобр.

		if(4==NP)
		{							//Если всего 4 точки - то пусть так и остается
			for(long i=0;i<Size;i++)
				X_co[i]=Y_co[i]=0;

			KnownCoeffs=0;
			Ready=1;
			return 0;
		};

		double x,y;
		
		double *Matr;		//Переопределенная матрица системы
		Matr=new double[NumEq*NumC];	// (NumEq)x(NumC)
		
		double *Rght;		//Правая часть
		Rght=new double[NumEq];

		double *X=new double[NumC];	//Ответ

		long i,j;

		double L;
		for(i=0;i<NP;i++){	//Заполняем
			for(j=0;j<KnownCoeffs;j++)
			{
				Matr[2*i*NumC+j]=Basis_Function(Basis,j,P1[i]);
				Matr[2*i*NumC+j+KnownCoeffs]=0;
				Matr[(2*i+1)*NumC+j]=0;
				Matr[(2*i+1)*NumC+j+KnownCoeffs]=Basis_Function(Basis,j,P1[i]);
			}
			Tr_Base(P1[i],x,y,L);
			Rght[2*i]=P2[i].x-x;		//Разница между проективным и реалом
			Rght[2*i+1]=P2[i].y-y;
		}

		CLeastSquareSolver LS;
		LS.Reset(NumEq,NumC,Matr);	//И решаем
		if(!LS.Solve(X,Rght)){
			delete[]Matr;
			delete[]Rght;
			delete[]X;
			return 2;
		}
		
	//	long s=KnownCoeffs*sizeof(double);
	/*	CopyMemory(X_co,X,s);
		CopyMemory(Y_co,X+s,s);*/
		for(i=0;i<KnownCoeffs;i++){
			X_co[i]=X[i];
			Y_co[i]=X[KnownCoeffs+i];
		}

		Ready=1;

		delete[]Matr;
		delete[]Rght;
		delete[]X;

		return 0;
	}

	long CSuperTransition::Trans(POINT P, POINT &R, double &L) const
	{
		//if (!Ready) return -1;
		long i;
		double x,y;
		if(Tr_Base(P,x,y,L))
							return 1;

		for(i=0;i<KnownCoeffs;i++)
		{
			x+=X_co[i]*Basis_Function(Basis,i,P);
			y+=Y_co[i]*Basis_Function(Basis,i,P);
		}

		R.x = (long)x;
		R.y = (long)y;

		return 0;
	}

	long CSuperTransition::Tr_Base(POINT P, double&x, double&y, double &L) const
	{
		L = BaseCalc.Result[2][0]*P.x + BaseCalc.Result[2][1]*P.y + BaseCalc.Result[2][2];

		if (0==L) return 1;

		x=(BaseCalc.Result[0][0]*P.x + BaseCalc.Result[0][1]*P.y + BaseCalc.Result[0][2])/L;
		y=(BaseCalc.Result[1][0]*P.x + BaseCalc.Result[1][1]*P.y + BaseCalc.Result[1][2])/L;
		return 0;
	}

	double CSuperTransition::BF_2(long N, POINT P) const
	{
		double x=(double)P.x;
		double y=(double)P.y;
		switch(N){
		case 0:return x*x/w/w;break;
		case 1:return y*y/h/h;break;
		case 2:return x*y/h/w;break;
		case 3:return sin(x*_PI/w);break;
		case 4:return sin(y*_PI/h);break;
		case 5:return cos(x*_PI/w);break;
		case 6:return cos(y*_PI/h);break;
		}
		return 0;
	}

	double CSuperTransition::BF_3(long N, POINT P) const
	{
		double x=(double)P.x;
		double y=(double)P.y;
		switch(N){
		case 0:return x*x/w/w;break;
		case 1:return y*y/h/h;break;
		case 2:return x*y/h/w;break;

		case 3:return sin(x*_PI/w);break;
		case 4:return sin(y*_PI/h);break;
		case 5:return cos(x*_PI/w);break;
		case 6:return cos(y*_PI/h);break;
		
		case 7:return sin(2*x*_PI/w);break;
		case 8:return sin(2*y*_PI/h);break;
		case 9:return cos(2*x*_PI/w);break;
		case 10:return cos(2*y*_PI/h);break;

		case 11:return sin(3*x*_PI/w);break;
		case 12:return sin(3*y*_PI/h);break;
		case 13:return cos(3*x*_PI/w);break;
		case 14:return cos(3*y*_PI/h);break;

		case 15:return sin(4*x*_PI/w);break;
		case 16:return sin(4*y*_PI/h);break;
		case 17:return cos(4*x*_PI/w);break;
		case 18:return cos(4*y*_PI/h);break;
		}
		return 0;

	}



	double CSuperTransition::BF_4(long N, POINT P) const
	{
		double x=(double)P.x;
		double y=(double)P.y;
		switch(N){
		case 0:return x*y/w/h;break;
		case 1:return y*y/h/h;break;
		case 2:return x*x/w/w;break;

		case 3:return sin(x*_PI/w);break;
		case 4:return sin(y*_PI/h);break;
		case 5:return cos(x*_PI/w);break;
		case 6:return cos(y*_PI/h);break;
		
		case 7:return sin(Peak(2,w,x)*_PI/w);break;
		case 8:return sin(Peak(2,h,y)*_PI/h);break;
		case 9:return cos(Peak(2,w,x)*_PI/w);break;
		case 10:return cos(Peak(2,h,y)*_PI/h);break;

	/*	case 11:return sin(Peak(3,w,x)*_PI/w);break;
		case 12:return sin(Peak(3,h,y)*_PI/h);break;
		case 13:return cos(Peak(3,w,x)*_PI/w);break;
		case 14:return cos(Peak(3,w,y)*_PI/h);break;

		case 15:return sin(Peak(4,w,x)*_PI/w);break;
		case 16:return sin(Peak(4,h,x)*_PI/h);break;
		case 17:return cos(Peak(4,w,x)*_PI/w);break;
		case 18:return cos(Peak(4,h,y)*_PI/h);break;*/
		}
		return 0;
	}




	double CSuperTransition::Peak(double max, long s, double x) const
	{
		if(x>s/2)x=s-x;
		return max*(2*x/s)*x;
	}

	long CSuperTransition::Serialize(BOOL _in, char *Buffer, long *Size)
	{
		char tmp[40];
		if(_in){	//Загружаем из буфера
	#define READ(t,v) 		{	k=0;	while(Buffer[k++]!=' '); \
			CopyMemory(tmp,Buffer,k);tmp[k]=0;v=ato##t(tmp); Buffer+=k;s+=k; }

			if(!Buffer)return 1;
			long i,k=0,s=0;

			READ(i,i);
			SetBas(i);
			READ(i,w);
			READ(i,h);

			for(i=0;i<9;i++)
			{
				READ(f,BaseCalc.Result[i/3][i%3]);
			}
			for(i=0;i<NumCo[Basis];i++)
				X_co[i]=Y_co[i]=0;
			READ(i,KnownCoeffs);
			for(i=0;i<KnownCoeffs;i++){
				READ(f,X_co[i]);
				READ(f,Y_co[i]);
			}
	 		Ready=1;
			*Size=s;
	#undef READ
		}else{	//Сохраняем в буфер
			long L=0,s=0;
			long i;
			
			s=sprintf(tmp,"%+i ",Basis);
			if(Buffer){
				CopyMemory(Buffer,tmp,s);
				Buffer+=s;
			}
			L+=s;
			s=sprintf(tmp,"%+i ",w);
			if(Buffer){
				CopyMemory(Buffer,tmp,s);
				Buffer+=s;
			}
			L+=s;
			s=sprintf(tmp,"%+i ",h);
			if(Buffer){
				CopyMemory(Buffer,tmp,s);
				Buffer+=s;
			}
			L+=s;

			for(i=0;i<9;i++){
				s=sprintf(tmp,"%+.29g ",BaseCalc.Result[i/3][i%3]);
				if(Buffer){
					CopyMemory(Buffer,tmp,s);
					Buffer+=s;
				}
				L+=s;
			}
			s=sprintf(tmp,"%+i ",KnownCoeffs);
			if(Buffer){
				CopyMemory(Buffer,tmp,s);
				Buffer+=s;
			}
			L+=s;
			for(i=0;i<KnownCoeffs;i++){
				s=sprintf(tmp,"%+.29g ",X_co[i]);
				if(Buffer){
					CopyMemory(Buffer,tmp,s);
					Buffer+=s;
				}
				L+=s;
				s=sprintf(tmp,"%+.29g ",Y_co[i]);
				if(Buffer){
					CopyMemory(Buffer,tmp,s);
					Buffer+=s;
				}
				L+=s;
			}
			*Size=L;
		}
		return 0;
	}

	void CSuperTransition::Copy(const CSuperTransition &ST)
	{
		Basis		= ST.Basis;
		KnownCoeffs = ST.KnownCoeffs;
		BaseCalc	= ST.BaseCalc;
		h			= ST.h;
		w			= ST.w;

		Size		= ST.Size;

		long s		= NumCo[Basis];

		if (X_co) delete[] X_co;	X_co = new double[s];	CopyMemory(X_co,ST.X_co,s*sizeof(double));

		if(Y_co)delete[] Y_co;		Y_co=new double[s];	CopyMemory(Y_co,ST.Y_co,s*sizeof(double));
		
		Ready=ST.Ready;
	}


	
		
	CSuperTransition& CSuperTransition::operator= (const CSuperTransition& st)
	{
		
		Copy(st);
		return *this;
	/*
		for (unsigned i=0; i<NumBas; ++i)
		{
			NumCo[i] = st.NumCo[i];
		}
		
		BaseCalc = st.BaseCalc;
			
		Basis = st.Basis;
		Size  = st.Size;			//С каким базисом работаем в данный момент
		
		if (X_co) delete X_co;
		if (Y_co) delete Y_co;

		for ()

		KnownCoeffs = st.KnownCoeffs;
		Ready = st.Ready;		//1 если преобр посчитано

		w = st.w;
		h = st.h;		//Размеры картинки. Для относительных координат типа в синусах
	*/
	}

		
	
}