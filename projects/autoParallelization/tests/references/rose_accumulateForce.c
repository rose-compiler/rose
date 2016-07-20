#include "omp.h" 

void AccumulateForce(int *idxBound,int *idxList,int len,double *tmp,double *force)
{
  
#pragma omp parallel for private (jj) firstprivate (len)
  for (register int ii = 0; ii <= len - 1; ii += 1) {
    int count = idxBound[ii + 1] - idxBound[ii];
    int *list = &idxList[idxBound[ii]];
    double sum = 0.0;
    
#pragma omp parallel for reduction (+:sum) firstprivate (count)
    for (register int jj = 0; jj <= count - 1; jj += 1) {
      int idx = list[jj];
      sum += tmp[list[jj]];
    }
    force[ii] += sum;
  }
  return ;
}
