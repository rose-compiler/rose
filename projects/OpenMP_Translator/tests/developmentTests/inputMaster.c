/* test 'omp master' with  preprocessing information
   also test 'omp barrier'
*/
#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

int main()
{
int nthreads;
#pragma omp parallel 
{
#if defined(_OPENMP)
  #pragma omp master
  {
  printf("I am thread %d out of %d threads\n", \
	omp_get_thread_num(), omp_get_num_threads()); 
  }
#endif

  #pragma omp barrier

#if defined(_OPENMP)
#pragma omp master
  nthreads = omp_get_num_threads();
#endif
}
  return 0;
}
