// Skip testing on 64 bit systems for now!
#ifndef __LP64__

#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include "omp_testsuite.h"

int
check_omp_num_threads (FILE * logFile)
{
  int failed = 0;
  int max_threads = 0;
  int threads;
  int nthreads;
  /* first we check how many threads are available */
#pragma omp parallel
  {
#pragma omp master
    max_threads = omp_get_num_threads ();
  }

  /* we increase the number of threads from one to maximum: */
  for (threads = 1; threads <= max_threads; threads++)
    {
      nthreads = 0;

#pragma omp parallel num_threads(threads) reduction(+:failed)
      {
	failed = failed + !(threads == omp_get_num_threads ());
#pragma omp atomic
	nthreads += 1;
      }
      failed = failed + !(nthreads == threads);
    }
  return !failed;
}

int
crosscheck_omp_num_threads (FILE * logFile)
{
  int failed = 0;
  int max_threads = 0;
  int threads;
  int nthreads;
  /* first we check how many threads are available */
#pragma omp parallel
  {
#pragma omp master
    max_threads = omp_get_num_threads ();
  }

  /* we increase the number of threads from one to maximum: */
  for (threads = 1; threads <= max_threads; threads++)
    {
      nthreads = 0;

#pragma omp parallel reduction(+:failed)
      {
	failed = failed + !(threads == omp_get_num_threads ());
#pragma omp atomic
	nthreads += 1;
      }
      failed = failed + !(nthreads == threads);
    }
  return !failed;
}

#else
  #warning "Not tested on 64 bit systems"
#endif

