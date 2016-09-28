#include <stdio.h>
#include "omp.h"
#include "omp_testsuite.h"

int
check_omp_in_parallel (FILE * logFile)
{
  /* checks that false is returned when called from serial region
     and true is returned when called within parallel region */
  int serial = 1;
  int isparallel = 0;
  serial = omp_in_parallel ();
#pragma omp parallel
  {
#pragma omp single
    {
      isparallel = omp_in_parallel ();
    }
  }
  return (!(serial) && isparallel);
}

int
crosscheck_omp_in_parallel (FILE * logFile)
{
  /* checks that false is returned when called from serial region
     and true is returned when called within parallel region */
  int serial = 1;
  int isparallel = 0;
  /*serial=omp_in_parallel(); */
#pragma omp parallel
  {
#pragma omp single
    {
      /*isparallel=omp_in_parallel(); */
    }
  }
  return (!(serial) && isparallel);
}
