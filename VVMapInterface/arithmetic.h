#ifndef _arithmetics_h_included_
#define _arithmetics_h_included_

#include <vector>

namespace arithmetic
{

	/// решает систему вида AX = B, методом Гаусса с выбором мах элемента n уравнений, n переменных
	bool SolveByGauss(int n,		///размер матрицы А
					  const double * A,	///матрица - размер:  n*n*sizeof(double)
					  const double * B,   ///вектор-столбец в правой части, размер:  n*sizeof(double)
					  double * X	///вектор-решение, размер:  n*sizeof(double)
					  );

	
	/// решает систему вида AX = B, n уравнений, m переменных, т<n
	/**
		система состоит из n линейных уравнений от m переменных.
		размер массива A - n*m, старший индекс - номер уравнения, младший - номер коэффициента
		размеры массивов B - n, X - m
	*/
	bool SolveByMNK	(int n,		///число уравнений
					 int m,		///число переменных
					 const double * A, ///A[n][m]
					 const double * B, ///B[n]
					 double * X  ///X[m]
					 );
	

}

#endif