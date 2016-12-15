// indirect array access: naive form
#include "omp.h" 

void foo1(int *indexSet,int N,int ax)
{
  double *xa3 = new double [N];
  
#pragma omp parallel for firstprivate (N,ax)
  for (int idx = 0; idx <= N - 1; idx += 1) {
    xa3[indexSet[idx]] += ax;
    xa3[indexSet[idx]] += ax;
  }
}
// indirect array access: transferred form

void foo2(int *indexSet,int N,int ax)
{
  double *xa3 = new double [N];
  
#pragma omp parallel for firstprivate (N,ax)
  for (int idx = 0; idx <= N - 1; idx += 1) {
    const int i = indexSet[idx];
    xa3[indexSet[idx]] += ax;
    xa3[indexSet[idx]] += ax;
  }
}
