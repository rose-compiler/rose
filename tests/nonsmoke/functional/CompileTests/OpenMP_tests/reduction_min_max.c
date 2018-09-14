// Contributed by Franke Ye @gatech.edu
#include <stdio.h>

int main()
{
  int a[10];
  int i, mn, mx;

#pragma omp parallel for reduction(min:mn)
  for(i=0 ; i<10 ; i++ )
    mn = mn < a[i] ? mn : a[i];

#pragma omp parallel for reduction(max:mx)
  for(i=0 ; i<10 ; i++ )
    mx = mx > a[i] ? mx : a[i];

  return 0;
}
