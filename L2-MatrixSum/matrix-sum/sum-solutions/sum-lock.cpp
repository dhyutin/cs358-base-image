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
  omp_lock_t sum_lock;
  omp_init_lock(&sum_lock);

  #pragma omp parallel for num_threads(T)
  for (int r = 0; r < N; r++)
  {
    for (int c = 0; c < N; c++)
    {
      omp_set_lock(&sum_lock);
      sum += M[r][c];
      omp_unset_lock(&sum_lock);
    }
  }

  omp_destroy_lock(&sum_lock);

  return sum;
}
