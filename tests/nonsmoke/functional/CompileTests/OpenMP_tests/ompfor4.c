/*
 * Dynamic schedule
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];

void foo(int lower, int upper, int stride)
{
  int i;
#pragma omp for schedule(dynamic)
  for (i=lower;i>upper;i-=stride)
  {
    a[i]=i*2;
    printf("Iteration %2d is carried out by thread %2d\n",\
        i, omp_get_thread_num());
  }
}

int main(void)
{
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
   foo(0,20,3);
  }
  return 0;
}

