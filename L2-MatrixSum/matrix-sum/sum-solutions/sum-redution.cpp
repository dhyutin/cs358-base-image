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
  // NOTE: tell openMP we are performing a reduction with 
  // the sum variable and addition, and it will create a
  // lock-free solution based on thread-local storage:
  //
  #pragma omp parallel for reduction(+:sum) num_threads(T)
  for (int r = 0; r < N; r++)
  {
    for (int c = 0; c < N; c++)
    {
      sum += M[r][c];
    }
  }

  return sum;
}
