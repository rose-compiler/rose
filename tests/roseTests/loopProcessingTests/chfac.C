#include <stdlib.h>
#include "chfac.h"
void chlskyfac(double* a,int NMAX)
{
  double sqrt(double);
double fabs(double);
int i;
int j;
int k;
for (k=0; k<NMAX; k+=1)
{
  arr_ref_a(a,k,k) = sqrt(fabs(arr_ref_a(a,k,k)));
  for (i=k+1; i<NMAX; i+=1)
  {
    arr_ref_a(a,i,k) = arr_ref_a(a,i,k)*1.0/arr_ref_a(a,k,k);
  }
  for (j=k+1; j<NMAX; j+=1)
  {
    for (i=j; i<NMAX; i+=1)
    {
      arr_ref_a(a,i,j) = arr_ref_a(a,i,j)-(arr_ref_a(a,i,k)*arr_ref_a(a,j,k));
    }
  }
}
}
