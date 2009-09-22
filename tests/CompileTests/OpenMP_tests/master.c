/*A nasty example of a preprocessing information within a basic block*/
#include <stdio.h>
#include <omp.h>

void foo(int step, int niter)
{
  int nthreads ;
#pragma omp parallel firstprivate(niter) private(step)
  {
    for (step = 1; step <= niter; step++) {

      if (step%20 == 0 || step == 1) {
#pragma omp master      
        printf(" Time step %4d\n", step);
      }
    }
#if defined(_OPENMP)
#pragma omp master  
    nthreads = omp_get_num_threads();
#endif /* _OPENMP */
  } /* end parallel */
}

