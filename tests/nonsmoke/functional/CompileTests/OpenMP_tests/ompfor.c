/*
* default loop scheduling
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];
int main(void)
{
  int i;
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
#pragma omp for nowait
    for (i=0;i<2;i+=1)
    //for (i=0;i<20;i+=3)
    {
      a[i]=i*2;
      printf("Iteration %2d is carried out by thread %2d\n",\
          i, omp_get_thread_num());
    }
  }
  return 0;
}

