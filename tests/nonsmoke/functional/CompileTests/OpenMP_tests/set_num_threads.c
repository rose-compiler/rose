#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(void)
{
  int counter=0, nthreads;
#ifdef _OPENMP
  omp_set_num_threads(7);
#endif  
#pragma omp parallel 
  {
#pragma omp critical
    counter ++;
    nthreads = omp_get_num_threads();
  }
  printf("number threads is:%d\n",nthreads);
#ifdef _OPENMP
  assert(counter == 7);
#else
  assert (counter ==1 );
#endif  
  return 0;
}

