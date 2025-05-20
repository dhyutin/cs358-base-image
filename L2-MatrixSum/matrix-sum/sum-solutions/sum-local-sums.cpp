/* mm.cpp */

//
// Matrix sum implementation, summing the contents of an 
// NxN matrix.
//
#include <iostream>
#include <string>
#include <sys/sysinfo.h>
#include <omp.h>

#include "alloc2D.h"
#include "sum.h"

using namespace std;


//
// MatrixSum:
//
// Computes and returns the sum of an NxN matrix.
//
double MatrixSum(double** M, int N, int T)
{
  //
  // Setup:
  //
  cout << "Num cores: " << get_nprocs() << endl;
  cout << "Num threads: " << T << endl;
  cout << endl;

  double sum = 0.0;

  //
  // For every row and column, add to sum:
  //
  double* sums = new double[T];
  
  // 
  // initial T local sum variables:
  //
  for (int i = 0; i < T; i++)
    sums[i] = 0.0;

  //
  // threads sum into local variables, so no locking needed:
  //
  #pragma omp parallel for num_threads(T)
  for (int r = 0; r < N; r++)
  {
    int thread_id = omp_get_thread_num();  // 0, 1, 2, ...

    for (int c = 0; c < N; c++)
    {
      sums[thread_id] += M[r][c];
    }
  }

  //
  // after join, main thread sums the local results info
  // final sum:
  //
  for (int i = 0; i < T; i++)
    sum += sums[i];

  delete[] sums;

  return sum;
}
