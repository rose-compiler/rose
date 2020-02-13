/*
 * Contributed by Jeff Keasler
 *
 * Liao 2/10/2010
 * */
#include <omp.h> 
typedef double real8;

void foo(real8 *a,real8 *b,real8 *c,real8 *d,int len)
{
  int icol;
  int jrow;
  int l;
  for (l = 0; l <= len - 1; l += 1) {
    int l8 = l * 8;
    real8 e = d[l * 3 + 0];
    real8 f = d[l * 3 + 1];
    real8 g = d[l * 3 + 2];
    real8 h = b[l];
    real8 tmp[8];
    
#pragma omp parallel for private (icol) firstprivate (e,f,g)
    for (icol = 0; icol <= 7; icol += 1) {
      tmp[icol] = e * c[(icol + l8) * 4 + 1] + f * c[(icol + l8) * 4 + 2] + g * c[(icol + l8) * 4 + 3];
    }
    
#pragma omp parallel for private (icol,jrow) firstprivate (l8,h)
    for (jrow = 0; jrow <= 7; jrow += 1) {
      real8 hj1 = h * c[(jrow + l8) * 4];
      
#pragma omp parallel for private (icol) firstprivate (hj1)
      for (icol = 0; icol <= 7; icol += 1) {
        a[icol + (jrow + l8) * 8] += hj1 * tmp[icol];
      }
    }
  }
}
