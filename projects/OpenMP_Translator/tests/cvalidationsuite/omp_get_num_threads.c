#include <stdio.h>
#include "omp.h"
#include "omp_testsuite.h"


int
check_omp_get_num_threads (FILE * logFile)
{
  /* checks that omp_get_num_threads is equal to the number of
     threads */
  int nthreads = 0;
  int nthreads_lib = -1;
#pragma omp parallel
  {
#pragma omp critical
    {
      nthreads++;
    }
#pragma omp single
    {
      nthreads_lib = omp_get_num_threads ();
    }
  }				/* end of parallel */
  return nthreads == nthreads_lib;
}

int
crosscheck_omp_get_num_threads (FILE * logFile)
{
  /* checks that omp_get_num_threads is equal to the number of
     threads */
  int nthreads = 0;
  int nthreads_lib = -1;
#pragma omp parallel
  {
#pragma omp critical
    {
      nthreads++;
    }
#pragma omp single
    {
      /*nthreads_lib=omp_get_num_threads(); */
    }
  }				/* end of parallel */
  return nthreads == nthreads_lib;
}
