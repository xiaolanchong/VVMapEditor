// OverlapInfo.h: interface for the COverlapInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERLAPINFO_H__7FF809AA_17C8_4766_9D08_1DB409D8479F__INCLUDED_)
#define AFX_OVERLAPINFO_H__7FF809AA_17C8_4766_9D08_1DB409D8479F__INCLUDED_

//#include "TransitionCalculator.h"	// Added by ClassView


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_COLL_POINTS 128

POINT CP(long x,long y);

class COverlapInfo  
{
public:
	long CalcTrans(CTransition& T);
	double AvgDiscr();
	void Init(long MaxPts);
	POINT Trans(POINT P,BOOL Dir=TRUE);
	
	long N;						//# collocation points
	POINT P1[MAX_COLL_POINTS],P2[MAX_COLL_POINTS];	//colloc points arrays

	long hl,wl;
	long hr,wr;

	typedef double TMatrix[3][3];

	TMatrix B12,B21;	//Transition matrices 2->1 and 1->2
	double CalcTrans();			//Calculates B from P1 and P2
	
	BOOL Ready;					//TRUE if transition is calculated

	double *a,*b,*X;
	//Options:
	long bas,maxfunc;
	double ppf;

	COverlapInfo();
	virtual ~COverlapInfo();

private:
	CTransitionCalculator Calc;
};

#endif // !defined(AFX_OVERLAPINFO_H__7FF809AA_17C8_4766_9D08_1DB409D8479F__INCLUDED_)
