/* main.cpp */

//
// Matrix sum app
//
// Sums the contents of a random NxN matrix.
//
// Usage:
//   sum [-?] [-n MatrixSize] [-t NumThreads]
//
// Author:
//   Prof. Joe Hummel
//   Northwestern University
//

#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include <chrono>
#include <random>
#include <sys/sysinfo.h>

#include "alloc2D.h"
#include "sum.h"

using namespace std;


//
// Globals:
//
static int _matrixSize;
static int _numThreads;

//
// Function prototypes:
//
void CreateAndFillMatrix(int N, double** &M);
void CheckResults(int N, double** M, double sum);
void ProcessCmdLineArgs(int argc, char* argv[]);


//
// main:
//
int main(int argc, char *argv[])
{
	//
	// Set defaults, process environment & cmd-line args:
	//
	_matrixSize = 20000;
	_numThreads = get_nprocs();  // default to # of cores:

	ProcessCmdLineArgs(argc, argv);

	cout << "** Matrix Sum Application **" << endl;
    cout << endl;
	cout << "Matrix size: " << _matrixSize << "x" << _matrixSize << endl;

	//
	// Create and fill the matrix to sum:
	//
	double **M;
	CreateAndFillMatrix(_matrixSize, M);

	//
	// Start clock and multiply:
	//
    auto start = chrono::high_resolution_clock::now();

	double sum = MatrixSum(M, _matrixSize, _numThreads);
  
    auto stop = chrono::high_resolution_clock::now();
    auto diff = stop - start;
    auto duration = chrono::duration_cast<chrono::milliseconds>(diff);

	cout << "Sum: " << sum << endl;

	//
	// Done, check results and output timing:
	//
	CheckResults(_matrixSize, M, sum);

    cout << endl;
    cout << "** Done!  Time: " << duration.count() / 1000.0 << " secs" << endl;
	cout << "** Execution complete **" << endl;
    cout << endl;

	Delete2dMatrix(M);

	return 0;
}


//
// CreateAndFillMatrix:
//
// Creates an NxN matrix and fills with random values.
//
void CreateAndFillMatrix(int N, double** &M)
{
	M = New2dMatrix<double>(N, N);

	random_device rd;
	mt19937 generator(rd());

	int min = 1;
	int max = 32767;

	uniform_int_distribution<> distribute(min, max);

	for (int r = 0; r < N /*rows*/; r++)
		for (int c = 0; c < N /*cols*/; c++)
			M[r][c] = distribute(generator);
}


//
// Checks the results:
//
void CheckResults(int N, double** M, double sum)
{ 
	double seq_sum = 0.0;

	for (int r = 0; r < N; r++)
	  for (int c = 0; c < N; c++)
		seq_sum += M[r][c];

	if (fabs(sum - seq_sum) < 0.000001) 
	{
		cout << "Results are correct" << endl;
	}
	else 
	{
		cout << "** ERROR: matrix sum yielded incorrect results" << endl << endl;
		exit(0);
	}
}


//
// processCmdLineArgs:
//
void ProcessCmdLineArgs(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{

		if (strcmp(argv[i], "-?") == 0)  // help:
		{
			cout << "**Usage: sum [-?] [-n MatrixSize] [-t NumThreads]" << endl << endl;
			exit(0);
		}
		else if ((strcmp(argv[i], "-n") == 0) && (i+1 < argc))  // matrix size:
		{
			i++;
			_matrixSize = atoi(argv[i]);
		}
		else if ((strcmp(argv[i], "-t") == 0) && (i+1 < argc))  // # of threads:
		{
			i++;
			_numThreads = atoi(argv[i]);
		}
		else  // error: unknown arg
		{
			cout << "**Unknown argument: '" << argv[i] << "'" << endl;
			cout << "**Usage: sum [-?] [-n MatrixSize] [-t NumThreads]" << endl << endl;
			exit(0);
		}

	}//for
}
