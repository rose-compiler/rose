/*
* default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int main(void)
{
  int i,j;
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
#pragma omp for private(j)
    for (i=0;i<10;i++)
    {
      j = omp_get_thread_num();
      printf("Iteration %d, by thread %d\n", i, j);
    }
  }
  return 0;
}

