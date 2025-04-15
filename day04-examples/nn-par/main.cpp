/* main.cpp */

//
// Nearest neighbor app
//
// Usage:
//   nn [-?] [-s Steps] [-n MatrixSize] [-t NumThreads]
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
#include <iomanip>
#include <sys/sysinfo.h>

#include "alloc2D.h"
#include "nn.h"

using namespace std;


//
// Globals:
//
static int _matrixSize;
static int _numThreads;
static int _numSteps;

//
// Function prototypes:
//
void CreateAndFillMatrix(int N, double** &M);
void CheckResults(double** M);
void ProcessCmdLineArgs(int argc, char* argv[]);


//
// main:
//
int main(int argc, char *argv[])
{
	//
	// Set defaults, process environment & cmd-line args:
	//
	_matrixSize = 2500;
	_numThreads = 1;  // sequential execution
	_numSteps = 200;

	ProcessCmdLineArgs(argc, argv);

	cout << "** Nearest Neighbor Application **" << endl;
  cout << endl;
	cout << "Matrix size: " << _matrixSize << "x" << _matrixSize << endl;

	//
	// Create and fill the matrices to multiply:
	//
	double **M;
	CreateAndFillMatrix(_matrixSize, M);

	//
	// Start clock and run alg:
	//
  auto start = chrono::high_resolution_clock::now();

	NearestNeighbor(M, _matrixSize, _numThreads, _numSteps);
  
  auto stop = chrono::high_resolution_clock::now();
  auto diff = stop - start;
  auto duration = chrono::duration_cast<chrono::milliseconds>(diff);

	//
	// Done, check results and output timing:
	//
	CheckResults(M);

  cout << endl;
  cout << "** Done!  Time: " << duration.count() / 1000.0 << " secs" << endl;
	cout << "** Execution complete **" << endl;
  cout << endl;

	Delete2dMatrix(M);

	return 0;
}


void CreateAndFillMatrix(int N, double** &M)
{
	M = New2dMatrix<double>(N, N);

	//
	// M looks like:
	//   1  2  3  4  ...  N
	//   2  3  4  5  ...  N+1
	//   .  .  .  .  ...  .
	//   .  .  .  .  ...  .
	//
	for (int r = 0; r < N /*rows*/; r++)
		for (int c = 0; c < N /*cols*/; c++)
			M[r][c] = rand();
}


//
// Checks the results against some expected results:
//
void CheckResults(double** M)
{ 
	int N = _matrixSize;

	//cout << std::fixed << std::setprecision(12) << M[1][1] << endl;
	//cout << std::fixed << std::setprecision(12) << M[1][N - 2] << endl;
	//cout << std::fixed << std::setprecision(12) << M[N - 2][1] << endl;
	//cout << std::fixed << std::setprecision(12) << M[N - 2][N - 2] << endl;

	if (_matrixSize == 2500 && _numSteps == 200)
	{
		double TL = 1226133099.128899812698;
		double TR = 1027718253.657175898552;
		double BL = 809010356.174453377724;
		double BR = 1189687442.818380355835;

		bool b1 = (fabs(M[1][1] - TL) < 0.0000001);
		bool b2 = (fabs(M[1][N - 2] - TR) < 0.0000001);
		bool b3 = (fabs(M[N - 2][1] - BL) < 0.0000001);
		bool b4 = (fabs(M[N - 2][N - 2] - BR) < 0.0000001);

		if (!b1 || !b2 || !b3 || !b4)
		{
			cout << "** ERROR: nearest neighbor yielded incorrect results" << endl << endl;
			exit(0);
		}
		else
		{
			cout << "Results are correct." << endl;
		}
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
			cout << "**Usage: nn [-?] [-s Steps] [-n MatrixSize] [-t NumThreads]" << endl << endl;
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
		else if ((strcmp(argv[i], "-s") == 0) && (i + 1 < argc))  // # of steps:
		{
			i++;
			_numSteps = atoi(argv[i]);
		}
		else  // error: unknown arg
		{
			cout << "**Unknown argument: '" << argv[i] << "'" << endl;
			cout << "**Usage: nn [-?] [-s Steps] [-n MatrixSize] [-t NumThreads]" << endl << endl;
			exit(0);
		}

	}//for
}
