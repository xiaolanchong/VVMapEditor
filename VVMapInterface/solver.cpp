#include "stdafx.h"
#include "solver.h"


		CLeastSquareSolver::CLeastSquareSolver() :A(0)
		{
		}

		CLeastSquareSolver::~CLeastSquareSolver()
		{
			if(A)delete [] A;
		}

		#define _A(i,j) iA[j*M+i]
		#define _At(i,j) A[j*N+i]
		#define _B(i,j) B[j*M+i]

		void CLeastSquareSolver::Reset(long n, long m, double *iA)
		{
			N=n;M=m;

			int i,j,k;
			if(A)delete[]A;
			A=new double[n*m];
			
			for(i=0;i<M;i++)for(j=0;j<N;j++)_At(j,i)=_A(i,j);

			double *B=new double[M*M];
			for(i=0;i<M;i++)
				for(j=0;j<M;j++){
					_B(i,j)=0;
					for(k=0;k<N;k++)
						_B(i,j)+=_At(k,j)*_A(i,k);
				}
			CGS.Reset(M,B);
			delete [] B;
		}

		bool CLeastSquareSolver::Solve(double *x, double *b)
		{
			int i,j;
			double *B=new double[M];
			for(i=0;i<M;i++){
				B[i]=0;
				for(j=0;j<N;j++)
					B[i]+=b[j]*_At(j,i);
			}
			bool OK = CGS.Solve(x,B);
			//if(!CGS.Solve_H(x,B))return CGS.Solve(x,B);
			delete [] B;
			return OK;
		}


		CGaussSolver::CGaussSolver() :A(0)
		{
		}

		CGaussSolver::~CGaussSolver()
		{
			if(A)delete [] A;
		}

		void CGaussSolver::Reset(long n, double *iA)
		{
			N=n;
			if(A)delete[]A;
			A=new double[N*N];
			memcpy(A,iA,N*N*sizeof(double));
		}

		#define ABS(a) (a>0?a:-a)
		const double eps=1e-16;
		bool CGaussSolver::Solve(double *x, double *_b)
		{
			int i,j,k;
			double c;
			b=_b;
			for(k=0;k<N-1;k++){
				Remap(k);
				c=a(k,k);
				if(ABS(c)<eps)return false;
				for(i=k;i<N;i++)
					a(i,k)/=c;
				b[k]/=c;
				for(j=k+1;j<N;j++){
					c=a(k,j);
					for(i=0;i<N;i++){
						a(i,j)-=a(i,k)*c;
					}
					b[j]-=b[k]*c;
				}
			}
			//Матрица треугольная с 1 на диаг. 
			for(k=N-1;k>=0;k--){
				for(j=N-1;j>k;j--)
					b[k]-=x[j]*a(j,k);
				if(ABS(a(k,k))<eps) return false;
				x[k]=b[k]/a(k,k);
			}
			return true;

		}

		void CGaussSolver::Remap(long k)
		{
			long i,j,j0=k;
			double Max=-1;
			for(j=k;j<N;j++)if(Max<ABS(a(k,j))){
				Max=ABS(a(k,j));j0=j;
			}

			for(i=0;i<N;i++){
				Max=a(i,k);
				a(i,k)=a(i,j0);
				a(i,j0)=Max;
			}
			Max=b[k];b[k]=b[j0];b[j0]=Max;
		#undef ABS
		}



