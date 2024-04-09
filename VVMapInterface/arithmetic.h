#ifndef _arithmetics_h_included_
#define _arithmetics_h_included_

#include <vector>

namespace arithmetic
{

	/// ������ ������� ���� AX = B, ������� ������ � ������� ��� �������� n ���������, n ����������
	bool SolveByGauss(int n,		///������ ������� �
					  const double * A,	///������� - ������:  n*n*sizeof(double)
					  const double * B,   ///������-������� � ������ �����, ������:  n*sizeof(double)
					  double * X	///������-�������, ������:  n*sizeof(double)
					  );

	
	/// ������ ������� ���� AX = B, n ���������, m ����������, �<n
	/**
		������� ������� �� n �������� ��������� �� m ����������.
		������ ������� A - n*m, ������� ������ - ����� ���������, ������� - ����� ������������
		������� �������� B - n, X - m
	*/
	bool SolveByMNK	(int n,		///����� ���������
					 int m,		///����� ����������
					 const double * A, ///A[n][m]
					 const double * B, ///B[n]
					 double * X  ///X[m]
					 );
	

}

#endif