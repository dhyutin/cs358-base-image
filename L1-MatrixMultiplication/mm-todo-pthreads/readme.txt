NOTE: initially, this is the sequential version. The goal is to use pthreads to safely parallelize. Focus on "mm.cpp", and use pthreads library to parallelize (think fork-join). Don't forget #include "pthread.h"

To build debug or optimized version:

  make debug => mm

  make opt   ==> mm-o

To run:

  mm [-?] [-n MatrixSize] [-t NumThreads]

  mm-o [-?] [-n MatrixSize] [-t NumThreads]