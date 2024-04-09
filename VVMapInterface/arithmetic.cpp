#include "stdafx.h"


#include "arithmetic.h"

#include "memleak.h"

namespace arithmetic
{
	/* Функция, реализующая метод Гаусса */
	#define IND2(i,j) ((i-1)*n + j - 1)
	#define IND1(i) (i-1)

	bool SolveByGauss(int n, const double * _A, const double * _B, /*вектор - решение*/ double *X)
	{

	int i, j, k, l;
	double q, m, t;

	std::vector <double> A (_A, _A + n*n);
	std::vector <double> B (_B, _B + n);


	for (k=1; k <= n-1; k++)
	{
		// Ищем строку l с максимальным элементом в k-ом столбце
		l = 0; m = 0;

			for (i=k; i<=n; i++)
			if (fabs(A[IND2(i, k)]) > m)
			{
				 m = fabs(A[IND2(i, k)]);
				 l = i;
			}


	// Если у всех строк от k до n элемент в k-м столбце нулевой,
	//то система не имеет однозначного решения 

			if (!l)
				return false;
			
	// Меняем местом l-ую строку с k-ой }

			if (l != k)
			{
				 for (j = 1; j<=n; j++)
				 {
						 t = A[IND2(k, j)];
						 A[IND2(k, j)] = A[IND2(l, j)];
						 A[IND2(l, j)] = t;
				 }

				 t = B[IND1(k)];
				 B[IND1(k)] = B[IND1(l)];
				 B[IND1(l)] = t;

			}//if

	// Преобразуем матрицу 

	  for (i = k+1; i <= n; i++)
	  {
		 q = A[IND2(i, k)] / A[IND2(k, k)];
		 for (j = 1; j <= n; j++)
			if (j == k)
				A[IND2(i, j)] = 0;
			else 
				A[IND2(i, j)] = A[IND2(i, j)] - q * A[IND2(k, j)];

		 B[IND1(i)] = B[IND1(i)] - q * B[IND1(k)];
	  }   //for
	}

	// Вычисляем решение 

	X[IND1(n)] = B[IND1(n)] / A[IND2(n, n)];

	   for (i = n-1; i >= 1; i--)
	   {
			t = 0;
			for (j = 1; j <= n-i; j++)
					 t = t + A[IND2(i, i + j)] * X[IND1(i + j)];
			X[IND1(i)] = (1 / A[IND2(i, i)]) * (B[IND1(i)] - t);
	   }

	 return true;

	}   //func
	///*************************************************************************************

	#undef IND2
	#define IND2(i,j) (i*m + j)

	#undef IND1
	#define IND1(i) (i)

	bool SolveByMNK(int n, int m, const double * A, const double * B, double * X)
	//система состоит из n линейных уравнений от m переменных.
	//размер массива A - n*m, старший индекс - номер уравнения, младший - номер коэффициента
	// размеры массивов B - n, X - m

	{
 
		int i,j,k;

		// Рассчет АтА/////////////////////////
		std::vector <double> AtA(m*m); 
		std::vector <double> AtB(m);


		for (i=0; i <= m-1; i++)
		for (j=0; j <= m-1; j++)
		{
			   AtA[IND2(i,j)] = 0;
			   for (k=0; k <= n-1; k++)
					AtA[IND2(i,j)] = AtA[IND2(i,j)] + A[IND2(k,i)]*A[IND2(k,j)];
		}
		////////////////////////////////////////

		for (i=0; i <= m-1; i++)
		{
				AtB[IND1(i)] = 0;
				for (j=0; j <= n-1; j++)
					 AtB[IND1(i)] = AtB[IND1(i)] + A[IND2(j,i)]*B[IND1(j)];
		}

		////////////////////////////////////////


		////////////////////////////////////////

		return SolveByGauss(m, &AtA[0], &AtB[0], X);
	}
}
//*******************************************************************************