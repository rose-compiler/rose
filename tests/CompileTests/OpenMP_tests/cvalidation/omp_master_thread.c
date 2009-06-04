#include <stdio.h>
#include <omp.h>
#include "omp_testsuite.h"

int
check_omp_master_thread (FILE * logFile)
{
  int nthreads = 0;
  int executing_thread = -1;
#pragma omp parallel
  {
#pragma omp master
    {
#pragma omp critical
      {
	nthreads++;
      }
      executing_thread = omp_get_thread_num ();

    }				/* end of master */
  }				/* end of parallel */
  return ((nthreads == 1) && (executing_thread == 0));
}

int
crosscheck_omp_master_thread (FILE * logFile)
{
  int nthreads = 0;
  int executing_thread = -1;
#pragma omp parallel
  {

    {
#pragma omp critical
      {
	nthreads++;
      }
      executing_thread = omp_get_thread_num ();

    }				/* end of master */
  }				/* end of parallel */
  return ((nthreads == 1) && (executing_thread == 0));
}
