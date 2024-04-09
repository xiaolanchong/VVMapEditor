#ifndef _SOLVER_H__
#define _SOLVER_H__

#include <windows.h>

		struct my_point
		{
			int x;
			int y;
			my_point() : x(0), y(0) {}
			my_point(int _x, int _y) : x(_x), y(_y) {}

		};

		//typedef my_point POINT;

		class CGaussSolver  
		{
		public:
			void Remap(long k);
			bool Solve(double *x,double*_b);
			void Reset(long n, double *iA);
			inline double& a(long i,long j){return A[j*N+i];}
			double * A, *b;
			long N;
			CGaussSolver();
			virtual ~CGaussSolver();
		};	


		class CLeastSquareSolver  
		{
		public:
			bool Solve(double *x, double *b);
			void Reset(long n, long m, double *iA);
			

			CLeastSquareSolver();
			virtual ~CLeastSquareSolver();

		private:

			CGaussSolver CGS;

			long N,M;
			double *A;

		};

		#define MAX_COLL_POINTS 128

		POINT CP(long x,long y);

	

#endif