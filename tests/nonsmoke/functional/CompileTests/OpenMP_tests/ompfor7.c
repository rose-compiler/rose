/*
 * test #define 
 * Liao 12/1/2010
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
#define P 4
void foo(int iend, int ist)
{
  int i=0;
  i= i+P;
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());

#pragma omp for nowait schedule(static,P)
    for (i=iend;i>=ist;i--)
    {
      printf("Iteration %d is carried out by thread %d\n",i, omp_get_thread_num());
    }
  }
}

