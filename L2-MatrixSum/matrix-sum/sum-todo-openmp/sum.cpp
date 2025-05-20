/* mm.cpp */

//
// Matrix sum implementation, summing the contents of an 
// NxN matrix.
//
#include <iostream>
#include <string>
#include <sys/sysinfo.h>

#include "alloc2D.h"
#include "sum.h"
#include "omp.h"

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
  omp_set_num_threads(T); 

  // // Do it separately for all threads
  // double* sums = new double[T];
  // for (int i = 0; i < T; i++){
  //   sums[i] = 0.0;
  // }

  // omp_lock_t sum_lock;
  // omp_init_lock(&sum_lock);
  //
  // For every row and column, add to sum:
  //
  #pragma omp parallel for reduction(+:sum)
  for (int r = 0; r < N; r++)
  {
    // int id = omp_get_thread_num();

    for (int c = 0; c < N; c++)
    {
      // omp_set_lock(&sum_lock);
      sum += M[r][c];
      // omp_unset_lock(&sum_lock);
    }
  }
  // omp_destroy_lock(&sum_lock);

  // for(int i = 0; i < T; i++){
  //   sum += sums[i];
  // }

  // delete[] sums;
  
  return sum;
}
