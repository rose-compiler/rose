#if 1
#include "omp.h" 

void foo1(double o1[],double c[],int len)
{
  int i;
  
#pragma omp parallel for private (i) firstprivate (len)
  for (i = 0; i <= len - 1; i += 1) {
    double volnew_o8 = 0.5 * c[i];
    o1[i] = volnew_o8;
  }
}
#endif
#if 1

void goo(double *o1,double *o2,double *a,double *b,double *c,int **idx,int len)
{
  int i;
  for (i = 0; i <= len - 1; i += 1) {
    int ii;
    const int *lidx = idx[i];
    double volnew_o8 = 0.5 * c[i];
    double volold_o8 = 0.5 * a[i] * b[i];
    for (ii = 0; ii <= 5; ii += 1) {
      int llidx = lidx[ii];
      o1[llidx] += volnew_o8;
      o2[llidx] += volold_o8;
    }
  }
}
#endif
