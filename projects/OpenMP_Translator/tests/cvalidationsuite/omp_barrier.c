#include <stdio.h>
#include <unistd.h>
#include <omp.h>
#include "omp_testsuite.h"
#include "omp_my_sleep.h"


int
check_omp_barrier (FILE * logFile)
{
  int result1 = 0;
  int result2 = 0;
#pragma omp parallel
  {
    int rank;
    rank = omp_get_thread_num ();
    if (rank == 1)
      {
	my_sleep (1.);
	result2 = 3;
      }
#pragma omp barrier
    if (rank == 0)
      {
	result1 = result2;
      }
  }
  return (result1 == 3);
}

int
crosscheck_omp_barrier (FILE * logFile)
{
  int result1 = 0;
  int result2 = 0;
#pragma omp parallel
  {
    int rank;
    rank = omp_get_thread_num ();
    if (rank == 1)
      {
	my_sleep (1.);
	result2 = 3;
      }

    if (rank == 0)
      {
	result1 = result2;
      }
  }
  return (result1 == 3);
}
