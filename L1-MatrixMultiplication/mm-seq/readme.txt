Sequential version of matrix multiply, which creates two random NxN matrices, multiplies, and outputs the execution time.

To build debug or optimized version:

  make debug => mm

  make opt   ==> mm-o

To run:

  mm [-?] [-n MatrixSize] [-t NumThreads]

  mm-o [-?] [-n MatrixSize] [-t NumThreads]