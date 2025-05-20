/* nn.cpp */

//
// Nearest neighbor implementation.
//
#include <iostream>
#include <string>
#include <sys/sysinfo.h>

#include "alloc2D.h"
#include "nn.h"

using namespace std;


//
// NearestNeighbor:
//
// Performs nearest neighbor algorithm on M.
//
double nn(double tl, double above, double tr,
  double l, double center, double r,
  double ll, double below, double lr)
{
  return (tl + above + tr + l + center + r + ll + below + lr) / 9.0;
}

void NearestNeighbor(double**& M, int N, int T, int steps)
{
  //
  // Setup:
  //
  cout << "Num cores:   " << get_nprocs() << endl;
  cout << "Num threads: " << T << endl;
  cout << "Num steps:   " << steps << endl;
  cout << endl;

  //
  // create and initialize second matrix so we read from M
  // and write to M2. We have to initial the edge rows and
  // cols of the new matrix M2:
  //
  double** M2 = New2dMatrix<double>(N, N);

  for (int c = 0; c < N; c++) {  // first and last rows
    M2[0][c] = M[0][c];
    M2[N - 1][c] = M[N - 1][c];
  }

  for (int r = 0; r < N; r++) {  // first and last cols
    M2[r][0] = M[r][0];
    M2[r][N-1] = M[r][N-1];
  }

  //
  // For all rows and columns except those along the edges:
  //
  int step = 1;

  while (step < steps)
  {

#pragma omp parallel for num_threads(T)
    for (int r = 1; r < N - 1; r++)
    {
      for (int c = 1; c < N - 1; c++)
      {
        M2[r][c] = nn(M[r - 1][c - 1], M[r - 1][c], M[r - 1][c + 1],
          M[r][c - 1], M[r][c], M[r][c + 1],
          M[r + 1][c - 1], M[r + 1][c], M[r + 1][c + 1]);
      }
    }

    step++;

    //
    // swap matrix pointers so M points to the result we
    // just computed, and repeat:
    //
    double** temp = M;
    M = M2;
    M2 = temp;
  }
  
  //
  // Final result is in M. That result is returned via
  // the parameter, so we just need to free M2:
  //
  Delete2dMatrix(M2);
}
