/*
1. with and without omp for
2. default loop scheduling: implementation dependent
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];
int main(void)
{
  int i;
  //#pragma omp parallel for schedule (auto)
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());

#pragma omp for
    for (i=0;i<20;i++)
    {
      a[i]=i*2;
      printf("Iteration %2d is carried out by thread %2d\n",\
          i, omp_get_thread_num());
    }
  }
}

