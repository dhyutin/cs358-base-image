/* mergesort.cpp */

//
// Mergesort.
//
#include <iostream>
#include <sys/sysinfo.h>

#include "mergesort.h"

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

  mergesort(A, 0, N-1, T);
}


//
// mergesort:
//
void mergesort(double arr[], int left, int right, int T)
{
  if (left < right) {

    int mid = left + (right - left) / 2;

    mergesort(arr, left, mid, T);

    mergesort(arr, mid + 1, right, T);

    merge(arr, left, mid, right);

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
