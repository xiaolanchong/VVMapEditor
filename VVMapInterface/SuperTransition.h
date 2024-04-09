// SuperTransition.h: interface for the CSuperTransition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUPERTRANSITION_H__12E3FB26_DCD5_4AE8_A0FF_85FCBFD09BAF__INCLUDED_)
#define AFX_SUPERTRANSITION_H__12E3FB26_DCD5_4AE8_A0FF_85FCBFD09BAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include<windows.h>
//#include "stdafx.h"
#include "TransitionCalculator.h"	// Added by ClassView

	//Число опредленных функциональных базисов
#define NumBas 4

class CSuperTransition  
{
	
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


	void Copy(CSuperTransition &ST);

	long Serialize(BOOL _in,char *Buffer,long *Size);
	
	long Calculate(long NP,POINT *P1,POINT *P2,double pff=5.);

	long KnownCoeffs;
	CTransitionCalculator BaseCalc;

	long Trans(POINT P,POINT &R, double &L) const;
	
		//Найти преобр
	void SetBas(long N);
		//Выбрать базис
		
	long Basis,Size;			//С каким базисом работаем в данный момент
	double *X_co,*Y_co;			//Коэффициэнты разложения преобразования Х и Y
	
	BOOL Ready;		//1 если преобр посчитано

	long w,h;		//Размеры картинки. Для относительных координат типа в синусах

	CSuperTransition();
	virtual ~CSuperTransition();

	
};

#endif // !defined(AFX_SUPERTRANSITION_H__12E3FB26_DCD5_4AE8_A0FF_85FCBFD09BAF__INCLUDED_)










































