/* mergesort.cpp */

//
// Mergesort.
//
#include <iostream>
#include <sys/sysinfo.h>

#include "mergesort.h"
#include "omp.h"
using namespace std;


// 
// function prototypes:
//
void mergesort(double arr[], int left, int right, int T);
void merge(double arr[], int left, int mid, int right);


//
// do_mergesort:
//
void do_mergesort(double* A, int N, int T)
{
  cout << "Num cores: " << get_nprocs() << endl;
  cout << "Num threads: " << T << endl;
  cout << endl;

#pragma omp parallel num_threads(T)
  {
    #pragma omp single
    {
      mergesort(A, 0, N - 1, T);
    }
  }
}




//
// mergesort:
//

static int numthreads = 0;

void mergesort(double arr[], int left, int right, int T)
{
  if (left < right) {

    int mid = left + (right - left) / 2;

    // serial
    if(numthreads >= T){
      mergesort(arr, left, mid, T);

      mergesort(arr, mid + 1, right, T);

      merge(arr, left, mid, right);
    }
    // parallel
    else{

#pragma omp atomic
numthreads += 2;

#pragma omp task
mergesort(arr, left, mid, T);

#pragma omp task
mergesort(arr, mid+1, right, T);

#pragma omp taskwait
merge(arr, left, mid, right);

#pragma omp atomic
numthreads -= 2;

      
    }

  }
}


//
// merge:
//
void merge(double arr[], int left, int mid, int right)
{
  int n1 = mid - left + 1;
  int n2 = right - mid;

  double* L = new double[n1];
  double* R = new double[n2];

  for (int i = 0; i < n1; i++)
    L[i] = arr[left + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j];

  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    }
    else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }

  delete[] L;
  delete[] R;
}
