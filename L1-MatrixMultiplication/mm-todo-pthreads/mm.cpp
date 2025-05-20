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



#include "pthread.h"

using namespace std;

//
// struct for communicating with thread-based implementation:
//
struct ThreadInfo {
  int      ID;
  int      NumThreads;
  int      N;
  double** A;
  double** B;
  double** C;

  ThreadInfo(int id, int t, int n, double** a, double** b, double** c)
   : ID(id), NumThreads(t), N(n), A(a), B(b), C(c)
  { }
};

static void* mm(void*);

//
// MatrixMultiply:
//
// Computes and returns C = A * B, where matrices are NxN. Does not make any attempt
// to optimimization the multiplication.
//
double** MatrixMultiply(double** const a, double** const b, int n, int t)
{
  double** c = New2dMatrix<double>(n, n);

  //
  // Setup:
  //
  int cores = get_nprocs();

  cout << "Num cores: " << cores << endl;
  cout << "Num threads: " << t << endl;
  cout << endl;

  //
  // Initialize target matrix in prep for summing:
  //
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      c[i][j] = 0.0;

  //
  // For starters, just execute using the main thread, nothing
  // in parallel:
  //
  

  pthread_t* threads = new pthread_t[t];


  // struct ThreadInfo* info;
  // info = new ThreadInfo(0 /*id*/, 
  //                       1 /*num threads*/, 
  //                       n /*matrix size*/,
  //                       a, b, c);
  
 //fork 
for (int i = 0; i < t; i++){
  struct ThreadInfo* info = new ThreadInfo(i, t, n, a, b, c);
  pthread_create(&threads[i], nullptr, mm, (void*) info);
}


  // Join
  for (int i = 0; i < t; i++){
    pthread_join(threads[i], nullptr);
  }

  delete[] threads;

  // mm(info);

  //
  // NOTE: mm() will delete the info object as part of the cleanup.
  //

  //
  // return pointer to result matrix:
  //
  return c;
}

//
// mm
//
// This function does the actual matrix multiplication, where each 
// thread does M rows, where M = N/T (size of matrix / # of threads).
// 
// When the computation is over, the info object passed will be 
// deleted.
// 
// Example: if there are 100 rows in the matrices and 4 threads, then
//   thread 0: rows 0..24
//   thread 1: rows 25..49
//   thread 2: rows 50..74
//   thread 3: rows 75..99
//
static void* mm(void* msg)
{
  struct ThreadInfo* info = (struct ThreadInfo*) msg;

  //
  // copy values out of struct so code is easier to read:
  //
  int id = info->ID;
  int T  = info->NumThreads;
  int N  = info->N;
  double** A = info->A;
  double** B = info->B;
  double** C = info->C;

  cout << "thread " << id << " starting" << endl;
  
  //
  // how many rows do we multiply?
  //
  int blockSize = N / T;
  int startRow = id * blockSize;
  int endRow = startRow + blockSize;

  // 
  // NOTE: if NumThreads does not divide evenly, the last thread
  // does the extra rows.
  //
  if (blockSize * T != N) { // did not evenly divide:
    int extra = N % T;
    
    if ((id + 1) == T)  // last thread in the group:
      endRow += extra;
  }
  
  //
  // For every row i of A and column j of B:
  //
  for (int i = startRow; i < endRow; i++)
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
  // free struct that was passed to us:
  //
  delete info;
  
  //
  // NOTE: the thread is not returning anything, answers are
  // in matrix C. So return nullptr.
  //
  return nullptr;
}
