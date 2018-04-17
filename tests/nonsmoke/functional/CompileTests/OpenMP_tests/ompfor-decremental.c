/*
 * default, decremental
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 

void foo(int lower, int upper, int stride)
{
  int i;
#pragma omp for
  for (i=lower;i>upper;i-=stride)
  {
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
   foo(9,-1,3);
  }
  return 0;
}

