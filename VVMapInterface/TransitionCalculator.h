// TransitionCalculator.h: interface for the CTransitionCalculator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSITIONCALCULATOR_H__B3370239_AB59_424F_9456_2DBE2965E9C0__INCLUDED_)
#define AFX_TRANSITIONCALCULATOR_H__B3370239_AB59_424F_9456_2DBE2965E9C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CTransitionCalculator  
{
public:
	double Calculate4(long *n);
	long Calculate();
	long N;						//# collocation points
	POINT P1[64];
	POINT P2[64];		//colloc points arrays

	double Result[3][3],Result1[3][3];

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

#endif // !defined(AFX_TRANSITIONCALCULATOR_H__B3370239_AB59_424F_9456_2DBE2965E9C0__INCLUDED_)
