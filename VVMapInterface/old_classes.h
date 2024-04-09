
#ifndef _OLD_CLASSES_H_included
#define _OLD_CLASSES_H_included

#include "serializer3.h"
#include "kpoint.h"
#include "memleak.h"
namespace trans
{

	#define MAX_COLL_POINTS 128

	POINT CP(long x,long y);
	//---------------------------------------------------------------------

	class CTransitionCalculator  
	{
	public:
		double Calculate4(long *n);
		long Calculate();
		long N;						//# collocation points
		POINT P1[64];
		POINT P2[64];		//colloc points arrays

		double Result[3][3],Result1[3][3];


		void ser(ser::CElement& e);
		CTransitionCalculator();
		virtual ~CTransitionCalculator();

	private:
		double a[64];
		double b[8];
		double X[8];
		double Delta(long a,long b);
		double Difference();


		inline double sq(double x){return x*x;}
	};


	//---------------------------------------------------------------------

	//#define NumBas 4

		class CProjectiveTrans;

	class CSuperTransition  
	{
		enum {NumBas = 4};
		
		friend class CProjectiveTrans;

		long NumCo[NumBas];		//Число базисных функций
		double Basis_Function(long Bas,long Num,POINT P) const;
		//Вычисляем Num-тую фукнкцию BAs-того базиса
		
		double BF_1(long N, POINT P) const;
		double BF_3(long N, POINT P) const;
		double BF_2(long N, POINT P) const;
		double BF_4(long N, POINT P) const;

		double Peak(double max, long s,double x) const;
		
		long Tr_Base(POINT P, double &x, double &y, double &L) const;
	public:
		
		long GetNumCo(int _NBas) const	{return NumCo[_NBas];}

		void Copy(const CSuperTransition &ST);

		long Serialize(BOOL _in,char *Buffer,long *Size);
		
		long Calculate(long NP,POINT *P1,POINT *P2,double pff=5.);

		
		CTransitionCalculator BaseCalc;

		long Trans(POINT P,POINT &R, double &L) const;
		
			//Найти преобр
		void SetBas(long N);
			//Выбрать базис
			
		long Basis, Size;			//С каким базисом работаем в данный момент
		double *X_co,*Y_co;			//Коэффициэнты разложения преобразования Х и Y
		
		long KnownCoeffs;
		BOOL Ready;		//1 если преобр посчитано

		long w,h;		//Размеры картинки. Для относительных координат типа в синусах

		CSuperTransition();
		virtual ~CSuperTransition();

		void ser(ser::CElement& e);
		
		CSuperTransition& operator= (const CSuperTransition& st);

		
	};
	//---------------------------------------------------------------------

	class COverlapInfo  
	{
	public:
		long CalcTrans(CSuperTransition& T);
		double AvgDiscr();
		void Init(long MaxPts);
		POINT Trans(POINT P,BOOL Dir=TRUE);
		
		long N;						//# collocation points
		POINT P1[MAX_COLL_POINTS],P2[MAX_COLL_POINTS];	//colloc points arrays

		long hl,wl;
		long hr,wr;

		typedef double TMatrix[3][3];

		TMatrix B12,B21;	//Transition matrices 2->1 and 1->2
		//double CalcTrans();			//Calculates B from P1 and P2
		
		BOOL Ready;					//TRUE if transition is calculated

		double *a,*b,*X;
		//Options:
		long bas,maxfunc;
		double ppf;

		COverlapInfo();
		virtual ~COverlapInfo();

		/////////////////////
		void CorrectLSign(CSuperTransition * T);

	private:
		CTransitionCalculator Calc;
	};
	//---------------------------------------------------------------------------------
	enum TransType
		{
			trProjective, trMeanWeight, trNeural
		};


	class CVVMOptions  
	{
	public:
		long Serialize(BOOL _in, char* Buffer, long *size,long vers);

	//тип преобразования 
	////////////////

		TransType type;
		
	//Предельные значения
		const long MAXBASIS;
		
		bool UseProj;
	//Опции SuperTransition
		//BOOL UseSuper;
		bool UseSuper;
		long bas,maxbas;
		double ppf;

		bool DoAdj;
		double RadOfAdj;

		/////дополнительные опции - А.Г.
	//// не сохраняются и не грузятся функцией Serialize


		bool UseWT; bool WT_inside;
	/////Neural options
		bool UseNeural; bool Neur_inside; 

		double LearnRatio; 
		
	// задает отношение точек для обучения к общему числу, остальные - для тестирования 
	/////////////////////////////////////
		
		
	////Support Points OPTIONS ////////////////////////////////////////////////
		bool use3DAlign;
	//////////////////////////////////////////	
		bool DrawMap;

		bool log_win;
		int col_lim;
		
		CVVMOptions();
		virtual ~CVVMOptions();

		CVVMOptions& operator= (const CVVMOptions& o);
		long old_load(const char *buf);
		long old_old_load(const char *pbuf);

	private:
		
	};

	//---------------------------------------------------------------------
}

#endif
