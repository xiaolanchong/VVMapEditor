// FourierTransformer.h: interface for the CFourierTransformer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOURIERTRANSFORMER_H__BEAE5F74_4D98_480A_8B20_9F9C00F8959A__INCLUDED_)
#define AFX_FOURIERTRANSFORMER_H__BEAE5F74_4D98_480A_8B20_9F9C00F8959A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "windows.h"

#define GD_USEOLDTRANS 1
class CAlgoComplex;
class CFourierTransformer  
{
public:
	long FFT1D(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward=TRUE);
	long FFT1DIter(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward=TRUE);
	long FFT1DNext(CAlgoComplex* _prev, long N, CAlgoComplex* _newarr, const CAlgoComplex& outvalue, const CAlgoComplex& invalue);

	long FFT2D(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward=TRUE);
	long FormComplexMatrix(PBYTE bits, PBITMAPINFOHEADER pbi, long c, PRECT rect, CAlgoComplex* outarr, long *pN=NULL);
	long FormBits(PBYTE bits, PBITMAPINFOHEADER pbi, long c, PRECT rect, CAlgoComplex* inarr, long N, double alfa=1, BOOL shift=FALSE);
	
	long Multiply(CAlgoComplex *dest, CAlgoComplex* source1,CAlgoComplex* source2, long N);
	long FindMaximum2D(CAlgoComplex *from, long N, long *x, long *y, double *pmin=NULL, double *pmax=NULL);
	long GetDisplacement(PBYTE bits1, PBYTE bits2, PBITMAPINFOHEADER pbih, long C, long sx, long sy, long N, long prevDx, long prevDy, long *dx, long *dy, CAlgoComplex *buffer=NULL, DWORD flags=0);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  bits1, bits2 - картинки, формат которых задается pbih
	//  C указывает, с какой цветовой плоскостью будет проводится работа 
	//		(С=0,1,2 - B, G, R, соответственно, C=4 - Gray Scale)
	//  sx, sy - координаты нижнего левого угла квадратного окна на bits1 
	//	N - ширина окна (sy отсчитывается от низа картинки)
	//  prevDx, prevDy - смещение окна на bits1 для картинки bits2. В результате вызова сравниваться будет содержимое
	//		окна (sx,h-sy-1,sx+N,h-sy-1-N) на bits1 и окна (sx-prevDx,h-sy+prevDy-1,sx-prevDx+N,h-sy+prevDy-N-1) на bits2
	//  dx, dy - адреса для возврата смещений изображений окон bits1 и bits2 относительно друг друга. Т.о.,
	//		смещение bits2 относительно bits1 состовляет (*dx+prevDx, *dy+prevDy)
	//  buffer - адрес для комплексного массива, содержащего промежуточные вычисления. Если равен NULL, то вся необходимая
	//		память выделиться и удалиться внутри функции. Чтобы избежать повторного выделения при постоянных запусках, 
	//		нужно передавать в качестве buffer массив из N*N*4 элементов
	//	flags - флаги, управляющие работой. Пока предусмотрен только один - GD_USEOLDTRANS. Если он поднят, то не будет
	//		выполнятся преобразование Фурье для bits1, оно будет взято из массива buffer. Флагом можно пользоваться только
	//		в случае, если уже выполнялась функция GetDisplacement с теми же параметрами bits1, sx, sy, N, при этом
	//		на вход подавался buffer. Только в этом случае в buffer останется необходимое преобразование
	
	CFourierTransformer();
	virtual ~CFourierTransformer();

private:	
	CAlgoComplex* from, *to, *temparr;
	long NRecurse;
	double ForwMul;
	double *tcos,*tsin;
	long *rev;
	long tableN, revN;
	double PI;

	void MakeTable(long N);
	void MakeRev(long N);
	void BitReverseCopy(long N);
	long FFTRecursion(long m, long r, CAlgoComplex* outarr);
	
};

#endif // !defined(AFX_FOURIERTRANSFORMER_H__BEAE5F74_4D98_480A_8B20_9F9C00F8959A__INCLUDED_)
