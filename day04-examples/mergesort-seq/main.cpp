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
#include <iomanip>
#include <locale>

#include "mergesort.h"

using namespace std;


//
// Globals:
//
static int _arraySize;
static int _numThreads;

//
// Function prototypes:
//
void CreateAndFillArray(int N, double* &A);
void CheckResults(int N, double* A);
void ProcessCmdLineArgs(int argc, char* argv[]);

void cout_with_commas(long a)
{
	long c = 1;

	if (a < 0) { a *= -1; cout << "-"; }
	while ((c *= 1000) < a);
	while (c > 1)
	{
		int t = (a % c) / (c / 1000);
		cout << (((c > a) || (t > 99)) ? "" : ((t > 9) ? "0" : "00")) << t;
		cout << (((c /= 1000) == 1) ? "" : ",");
	}
}

int main(int argc, char *argv[])
{
	//
	// Set defaults, process environment & cmd-line args:
	//
	_arraySize = 100000000;
	_numThreads = 1;  // sequential execution

	ProcessCmdLineArgs(argc, argv);

	cout << "** Mergesort Application **" << endl;
  cout << endl;

	cout << "Array size: ";
	cout_with_commas(_arraySize);
	cout << " columns" << endl;

	//
	// Create and fill the matrix to sum:
	//
	double *A;
	CreateAndFillArray(_arraySize, A);

	//
	// Start clock and multiply:
	//
  auto start = chrono::high_resolution_clock::now();

	do_mergesort(A, _arraySize, _numThreads);
  
  auto stop = chrono::high_resolution_clock::now();
  auto diff = stop - start;
  auto duration = chrono::duration_cast<chrono::milliseconds>(diff);

	//
	// Done, check results and output timing:
	//
	CheckResults(_arraySize, A);

  cout << endl;
  cout << "** Done!  Time: " << duration.count() / 1000.0 << " secs" << endl;
	cout << "** Execution complete **" << endl;
  cout << endl;

	delete[] A;

	return 0;
}


//
// CreateAndFillArray:
//
// Creates an array of size N and fills with random values.
//
void CreateAndFillArray(int N, double*& A)
{
	A = (double*) new double[N];
	if (A == nullptr) {
		cout << "**ERROR: out of memory" << endl;
		exit(0);
	}

	random_device rd;
	mt19937 generator(rd());

	long long min = 1;
	long long max = 1000000000000;

	uniform_int_distribution<long long> distribute(min, max);

	for (int c = 0; c < N; c++)
		A[c] = distribute(generator);
}


//
// Checks the results:
//
void CheckResults(int N, double* A)
{ 
	bool correct = true;

	for (int i = 0; i < N-1; i++)
	{
		if (A[i] > A[i + 1]) {
			correct = false;
			break;
		}
	}

	if (correct) 
	{
		cout << "Results are correct" << endl;
	}
	else 
	{
		cout << "** ERROR: at least one element is out of order" << endl << endl;
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
			cout << "**Usage: mergesort [-?] [-n ArraySize] [-t NumThreads]" << endl << endl;
			exit(0);
		}
		else if ((strcmp(argv[i], "-n") == 0) && (i+1 < argc))  // matrix size:
		{
			i++;
			_arraySize = atoi(argv[i]);
		}
		else if ((strcmp(argv[i], "-t") == 0) && (i+1 < argc))  // # of threads:
		{
			i++;
			_numThreads = atoi(argv[i]);
		}
		else  // error: unknown arg
		{
			cout << "**Unknown argument: '" << argv[i] << "'" << endl;
			cout << "**Usage: mergesort [-?] [-n ArraySize] [-t NumThreads]" << endl << endl;
			exit(0);
		}

	}//for
}
