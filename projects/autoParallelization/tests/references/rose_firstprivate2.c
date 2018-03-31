#include <omp.h> 

void goo(double *o1,double *c,int **idx,int len)
{
  int i;
  for (i = 0; i <= len - 1; i += 1) {
    int ii;
    const int *lidx = idx[i];
    double volnew_o8 = 0.5 * c[i];
    
#pragma omp parallel for private (ii) firstprivate (volnew_o8)
    for (ii = 0; ii <= 5; ii += 1) {
      int llidx = lidx[ii];
      o1[lidx[ii]] += volnew_o8;
    }
  }
}
