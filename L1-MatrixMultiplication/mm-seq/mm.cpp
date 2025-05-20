/* mm.cpp */

//
// Matrix multiplication implementation, computing C=A*B where A and B
// are NxN matrices. The resulting matrix C is therefore NxN.
//
#include <iostream>
#include <string>
#include <sys/sysinfo.h>

#include "alloc2D.h"
#include "mm.h"

using namespace std;


//
// MatrixMultiply:
//
// Computes and returns C = A * B, where matrices are NxN. No attempt is made
// to optimize the multiplication.
//
double** MatrixMultiply(double** const A, double** const B, int N, int T)
{
  double** C = New2dMatrix<double>(N, N);

  //
  // Setup:
  //
  cout << "Num cores: " << get_nprocs() << endl;
  cout << "Num threads: " << T << endl;
  cout << endl;

  //
  // Initialize target matrix in prep for summing:
  //
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      C[i][j] = 0.0;

  //
  // For every row i of A and column j of B:
  //
  #pragma omp parallel for num_threads(T)
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      for (int k = 0; k < N; k++)
      {
        C[i][j] += (A[i][k] * B[k][j]);
      }
    }
  }
  
  //
  // return pointer to result matrix:
  //
  return C;
}
