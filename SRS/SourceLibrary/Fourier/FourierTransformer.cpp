// FourierTransformer.cpp: implementation of the CFourierTransformer class.
//
//////////////////////////////////////////////////////////////////////

#include "FourierTransformer.h"
#include "AlgoComplex.h"
#include "math.h"

#ifdef DEBUG_NEW
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFourierTransformer::CFourierTransformer():PI(3.1415926535897932384626433832795)
{	
	from=NULL;
	to=NULL;	
	tcos=tsin=NULL; tableN=0;
	rev=NULL; revN=0;

}

CFourierTransformer::~CFourierTransformer()
{
	if (tcos) delete[] tcos;
	if (tsin) delete[] tsin;
	if (rev) delete[] rev;
}


void CFourierTransformer::MakeRev(long N)
{
	if (revN==N) return;
	revN=N;
	if (rev) delete[] rev;	rev=new long[N];
	long i,j;
	rev[0]=0;
	for (j=1; j<N; j*=2) {
		rev[j]=N/j/2;
		for (i=j+1; i<j*2; i++) {
			rev[i]=rev[j]+rev[i-j];
		}
	}
}

void CFourierTransformer::MakeTable(long N)
{
	if (tableN==N) return;		tableN=N;
	if (tcos) delete[] tcos;	tcos=new double[N];
	if (tsin) delete[] tsin;	tsin=new double[N];	
	for (long u=0; u<N; u++) {
		tcos[u]=cos(-2*PI*u/double(N));
		tsin[u]=sin(-2*PI*u/double(N));
	}
}
void CFourierTransformer::BitReverseCopy(long N) 
{
	for (long k=0; k<N; k++) to[rev[k]]=from[k];
}
long CFourierTransformer::FFT1DIter(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward) 
{
	from=_from;	to=_to; ForwMul=(forward)?(1):(-1);	
	if (N!=revN) {
		MakeRev(N);
		MakeTable(N);
	}
	BitReverseCopy(N);
	long m,j,k;
	CAlgoComplex wm,w,t;	
	for (m=2; m<=N; m*=2) {
		wm.re=tcos[N/m]; wm.im=ForwMul*tsin[N/m]; w=1;
		for (j=0; j<m/2; j++) {			
			for (k=j; k<N; k+=m) {
				t=w*to[k+m/2];	//u=to[k]; to[k]=u+t; to[k+m/2]=u-t;
				
				to[k].re+=t.re;					to[k].im+=t.im;					//to[k]+=t;				
				to[k+m/2].re=to[k].re-2*t.re;	to[k+m/2].im=to[k].im-2*t.im;	//to[k+m/2]=to[k]-t*2;
			}
			w*=wm;
		}
	}
	double dN=double(N);
	if (forward) for (j=0; j<N; j++) to[j]/=dN;
	return 0;
}
long CFourierTransformer::FFT1D(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward) 
{
	from=_from;	to=_to; NRecurse=N; ForwMul=(forward)?(1):(-1);
	if (N!=tableN) MakeTable(N);
	FFTRecursion(1,0,to);
	if (forward) for (long i=0; i<N; i++) to[i]/=double(N);
	return 0;
}

long CFourierTransformer::FFTRecursion(long m, long r, CAlgoComplex* outarr)
{	
	long M=NRecurse/(2*m);
	if (m==NRecurse) {
		outarr[0]=from[r];
	} else {		
		//CAlgoComplex mult;
		CAlgoComplex* evenarr=new CAlgoComplex[M];
		CAlgoComplex*  oddarr=new CAlgoComplex[M];
		FFTRecursion(2*m,r,evenarr);
		FFTRecursion(2*m,r+m,oddarr);
		long u;
		for (u=0; u<M; u++)	{
			//mult.re=tcos[u*m];
			//mult.im=ForwMul*tsin[u*m];
			//outarr[u]=evenarr[u]+oddarr[u]*mult;
			//outarr[u+M]=evenarr[u]-oddarr[u]*mult;
			outarr[u].re=evenarr[u].re+oddarr[u].re*tcos[u*m]-oddarr[u].im*ForwMul*tsin[u*m];
			outarr[u].im=evenarr[u].im+oddarr[u].re*ForwMul*tsin[u*m]+oddarr[u].im*tcos[u*m];
			outarr[u+M].re =evenarr[u].re*2-outarr[u].re;
			outarr[u+M].im=evenarr[u].im*2-outarr[u].im;
			
		}
		delete[] evenarr;
		delete[] oddarr;
	}
	return 0;
}

long CFourierTransformer::FFT2D(CAlgoComplex* _from, long N, CAlgoComplex* _to, BOOL forward) 
{		
	long i,j;
	CAlgoComplex swap;
	CAlgoComplex* tempTo=new CAlgoComplex[N*N];
	
	for (i=0; i<N; i++) FFT1DIter(_from+i*N,N,tempTo+i*N,forward);	
	for (j=0; j<N; j++) for (i=j+1; i<N; i++) {	swap=tempTo[j*N+i]; tempTo[j*N+i]=tempTo[i*N+j]; tempTo[i*N+j]=swap;}
	for (i=0; i<N; i++) FFT1DIter(tempTo+i*N,N,_to+i*N,forward);	
	for (j=0; j<N; j++) for (i=j+1; i<N; i++) {	swap=_to[j*N+i]; _to[j*N+i]=_to[i*N+j]; _to[i*N+j]=swap;}
	delete[] tempTo;	
	return 0;
}
long CFourierTransformer::FormComplexMatrix(PBYTE bits, PBITMAPINFOHEADER pbi, long c, PRECT rect, CAlgoComplex* outarr, long *pN)
{
	long h=pbi->biHeight;
	long size=pbi->biSizeImage, bpl=size/h, bpp=pbi->biBitCount/8;
	if (bpp<3) return -1;
	long W=rect->right-rect->left+1;
	long H=rect->bottom-rect->top+1;
	long i,j,N;
	N=min(W,H);
		
	if (c!=4) {			
		for (j=0; j<N; j++) {			
			for (i=0; i<N; i++)	{
				outarr[j*N+i]=double(bits[(j+rect->top)*bpl+(i+rect->left)*bpp+c]);
			}
		}		
	} else {
		double I;
		for (j=0; j<N; j++) for (i=0; i<N; i++)	{
			I=bits[(j+rect->top)*bpl+(i+rect->left)*bpp];
			I+=bits[(j+rect->top)*bpl+(i+rect->left)*bpp+1];
			I+=bits[(j+rect->top)*bpl+(i+rect->left)*bpp+2];
			I/=3;
			outarr[j*N+i]=I;
		}
	}
	if (pN) *pN=N;
	return 0;
}

long CFourierTransformer::FormBits(PBYTE bits, PBITMAPINFOHEADER pbi, long c, PRECT rect, CAlgoComplex* inarr, long N, double alfa, BOOL shift)
{
	long h=pbi->biHeight;
	long size=pbi->biSizeImage, bpl=size/h, bpp=pbi->biBitCount/8;
	if (bpp<3) return -1;
	long i,j,ni,nj, sh;
	double I;
	if (shift) {sh=N/2;} else {sh=0;}
	if (c!=4) {
		for (j=0; j<N; j++) for (i=0; i<N; i++)	{
			nj=(j+sh)%N; ni=(i+sh)%N;
			I=fabs(inarr[j*N+i])*alfa; if (I>255) I=255;
			bits[(nj+rect->top)*bpl+(ni+rect->left)*bpp+c]=BYTE(I);
		}
	} else {		
		for (j=0; j<N; j++) for (i=0; i<N; i++)	{
			nj=(j+sh)%N; ni=(i+sh)%N;
			I=fabs(inarr[j*N+i])*alfa; if (I>255) I=255;
			bits[(nj+rect->top)*bpl+(ni+rect->left)*bpp]=BYTE(I);
			bits[(nj+rect->top)*bpl+(ni+rect->left)*bpp+1]=BYTE(I);
			bits[(nj+rect->top)*bpl+(ni+rect->left)*bpp+2]=BYTE(I);			
		}
	}
	return 0;
	
}
long CFourierTransformer::Multiply(CAlgoComplex *dest, CAlgoComplex* source1,CAlgoComplex* source2, long N)
{
	for (long i=0; i<N; i++) {
		//dest[i]=(source1[i])*(~source2[i]);
		dest[i].re=source1[i].re*source2[i].re+source1[i].im*source2[i].im;
		dest[i].im=source1[i].im*source2[i].re-source1[i].re*source2[i].im;
	}
	return 0;
}
long CFourierTransformer::FindMaximum2D(CAlgoComplex *from, long N, long *x, long *y, double *pmin, double *pmax)
{
	long i,j;
	double maxr, minr=1e30, r;
	*x=-1; *y=-1; maxr=-1;
	for (j=0; j<N; j++) for (i=0; i<N; i++)	{
		r=(from[j*N+i]).square();
		if (r>maxr) {
			maxr=r; *x=i; *y=j;
		}
		minr=min(minr,r);
	}
	if (pmin) *pmin=sqrt(minr);
	if (pmax) *pmax=sqrt(maxr);
	return (*x!=-1)?0:(-1);
}
long CFourierTransformer::GetDisplacement(PBYTE bits1, PBYTE bits2, PBITMAPINFOHEADER pbih, long C, long sx, long sy, long N, long prevDx, long prevDy, long *dx, long *dy, CAlgoComplex *buffer, DWORD flags)
{
	if ((dx==NULL) || (dy==NULL) || (bits2==NULL) || ((bits1==NULL) && !(flags&GD_USEOLDTRANS)) ) return -2;
	*dx=0; *dy=0;
	long bitCount=0;
	long mask=1;
	while(mask) {
		if(mask&N) bitCount++;
		mask<<=1;
	};
	if (bitCount!=1) return -1;
	CAlgoComplex *source1, *source2, *ft1,*ft2;
	if (buffer==NULL) {
		source1=new CAlgoComplex[N*N];
		ft1=new CAlgoComplex[N*N];
		ft2=new CAlgoComplex[N*N];
		source2=new CAlgoComplex[N*N];
		flags=flags&(~GD_USEOLDTRANS);
	} else {
		source1=buffer;
		source2=buffer+N*N;
		ft1=buffer+2*N*N;
		ft2=buffer+3*N*N;
	}
	
	RECT r={sx,sy,sx+N-1,sy+N-1};	
	RECT r2={r.left-prevDx,r.top-prevDy,r.right-prevDx,r.bottom-prevDy};
	
	if (!(flags&GD_USEOLDTRANS)) FormComplexMatrix(bits1,pbih,C,&r,source1);
	FormComplexMatrix(bits2,pbih,C,&r2,source2);
	if (!(flags&GD_USEOLDTRANS)) FFT2D(source1,N,ft1);
	FFT2D(source2,N,ft2);
	Multiply(source2,ft1,ft2,N*N);
	FFT2D(source2,N,ft2,FALSE);
	FindMaximum2D(ft2,N,dx,dy);
	
	if (*dx>=N/2) *dx-=N;
	if (*dy>=N/2) *dy-=N;
	
	if (buffer==NULL) {
		delete [] source1;
		delete [] source2;
		delete [] ft1;
		delete [] ft2;
	}
	
	return 0;
}


long CFourierTransformer::FFT1DNext(CAlgoComplex* _prev, long N, CAlgoComplex* _newarr, const CAlgoComplex& outvalue, const CAlgoComplex& invalue)
{

	long i;
	CAlgoComplex deltag, mulv, q;
	deltag=invalue-outvalue;
	q.re=cos(2*PI/double(N));
	q.im=sin(2*PI/double(N));
	mulv=1;
	for (i=0; i<N; i++) {
		_newarr[i]=_prev[i]*mulv+deltag;
		mulv*=q;
	}
	return 0;		 
}