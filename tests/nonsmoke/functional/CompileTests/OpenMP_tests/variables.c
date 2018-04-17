/*
test input

  handling of variable scopes:
   private, firstprivate, lastprivate
   shared : from the same scope or from upper level scope
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include "omp.h"
#endif

int gi=0;
int m=1;
double mm=9.0;
#pragma omp threadprivate(m)
//#pragma omp threadprivate(mm)

int main(void)
{
  int i;
  int k_3;
  int gj=0;
  double j=0.5;
  // m=1;
  // mm=9.0;
  k_3=7;

#pragma omp parallel private (i) firstprivate(k_3) reduction(+:gi,gj) private(mm)
  {
    int k=1;
#ifdef  _OPENMP 
    i=omp_get_thread_num();
#endif
    printf("Hello,world! I am thread %d\n",i);
    //#pragma omp for shared(mm) 
    // shared () cannot be used with 'omp for' according to the specification 
#pragma omp for
    for (i=0;i<100;i++)
    {i+=mm;}

#pragma omp single 
    printf("mm is:%f\n",mm);
#pragma omp critical
    gi=i+j+k+k_3;
    // int i=5;
    gi+=i;
#pragma omp atomic
    j++;
    gj+=m+mm;

  }
  printf("gi is %d,gj is %d\n",gi,gj);
  return 0;
}

