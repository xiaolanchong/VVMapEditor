// AlgoComplex.h: interface for the CAlgoComplex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALGOCOMPLEX_H__769A7F5B_C1F2_485C_9303_A91810DB4492__INCLUDED_)
#define AFX_ALGOCOMPLEX_H__769A7F5B_C1F2_485C_9303_A91810DB4492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>


class CAlgoComplex  
{	
public:		
	double re, im;
	//CAlgoComplex(double _re=0, double _im=0): re(_re), im(_im) {;};	

	__forceinline CAlgoComplex& operator=(const CAlgoComplex& x) {re=x.re; im=x.im; return *this;}
	__forceinline CAlgoComplex& operator=(double x) {re=x; im=0; return *this;}

	__forceinline CAlgoComplex operator+(const CAlgoComplex& x) const {CAlgoComplex res; res.re=re+x.re; res.im=im+x.im; return res;} 
	__forceinline CAlgoComplex operator+(double x) const {CAlgoComplex res; res.re=re+x; return res; } 
	__forceinline CAlgoComplex& operator+=(const CAlgoComplex& x) {re+=x.re; im+=x.im; return *this;}
	__forceinline CAlgoComplex& operator+=(double x) {re+=x; return *this;}

	__forceinline CAlgoComplex operator-(const CAlgoComplex& x) const {CAlgoComplex res; res.re=re-x.re; res.im=im-x.im; return res;} 
	__forceinline CAlgoComplex operator-(double x) const {CAlgoComplex res; res.re=re-x; return res; } 
	__forceinline CAlgoComplex& operator-=(const CAlgoComplex& x) {re-=x.re; im-=x.im; return *this;}
	__forceinline CAlgoComplex& operator-=(double x) {re-=x; return *this;}
	
	static __forceinline CAlgoComplex constI() { CAlgoComplex res; res.re=0; res.im=1; return res;}
	__forceinline double square() const {return re*re+im*im;}

	double fabs() const;
	CAlgoComplex cexp() const;
	CAlgoComplex cln() const;

	__forceinline CAlgoComplex operator~() const {CAlgoComplex res; res.re=re; res.im=-im; return res;}
	__forceinline CAlgoComplex operator*(double x) const {CAlgoComplex res; res.re=re*x; res.im=im*x; return res;}
	__forceinline CAlgoComplex operator*(const CAlgoComplex& x) const {CAlgoComplex res; res.re=re*x.re-im*x.im; res.im=re*x.im+im*x.re; return res;}
	__forceinline CAlgoComplex& operator*=(double x) {re*=x; im*=x; return *this;}
	__forceinline CAlgoComplex& operator*=(const CAlgoComplex& x) {double tmpRe=re*x.re-im*x.im; im=re*x.im+im*x.re; re=tmpRe; return *this;}
	
	__forceinline CAlgoComplex operator/(double x) const {CAlgoComplex res; res.re=re/x; res.im=im/x; return res;}
	__forceinline CAlgoComplex operator/(const CAlgoComplex& x) const {return operator*(~x)/x.square();}
	__forceinline CAlgoComplex& operator/=(double x) {re/=x; im/=x; return *this;}
	__forceinline CAlgoComplex& operator/=(const CAlgoComplex& x) {return (*this=operator*(~x)/x.square());}
	

	
};
double __cdecl fabs(const CAlgoComplex& x);
CAlgoComplex __cdecl cexp(const CAlgoComplex& x);
CAlgoComplex __cdecl ln(const CAlgoComplex& x);


#endif // !defined(AFX_ALGOCOMPLEX_H__769A7F5B_C1F2_485C_9303_A91810DB4492__INCLUDED_)
