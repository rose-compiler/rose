#include <stdio.h>

#if defined(_OPENMP)
#include <omp.h>
#endif /* _OPENMP */

static double a[1000];

static void init(void)
{
  int i=0;
  i=i+5; 
#pragma omp for
  for (i=0;i<1000;i++)
  {
    a[i]=(double)i/2.0;
  }
}

int main(void){

#pragma omp parallel
 {
  init();
 }
 return 0;
}

