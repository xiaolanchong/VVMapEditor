// AlgoComplex.cpp: implementation of the CAlgoComplex class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4514)
#include "AlgoComplex.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


double __cdecl fabs(const CAlgoComplex& x) {	return x.fabs();}
CAlgoComplex __cdecl  cexp(const CAlgoComplex& x) {	return x.cexp();}
CAlgoComplex __cdecl  ln(const CAlgoComplex& x) {	return x.cln();}

CAlgoComplex CAlgoComplex::cexp() const { 
	CAlgoComplex res;
	double e=exp(re);
	res.re=e*cos(im);
	res.im=e*sin(im);
	return res;
}
CAlgoComplex CAlgoComplex::cln() const  { 
	double S=square(); 
	CAlgoComplex res;
	res.re=log(S); res.im=atan2(re/S,im/S);
	return res;
}

double CAlgoComplex::fabs() const {return sqrt(square());}

