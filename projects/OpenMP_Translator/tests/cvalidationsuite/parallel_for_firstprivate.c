#include <stdio.h>
#include <math.h>
#include "omp_testsuite.h"

int
check_parallel_for_firstprivate (FILE * logFile)
{
  int sum = 0;
  /*int sum0=0; */
  int known_sum;
  int i2 = 3;

  int i;
#pragma omp parallel for firstprivate(i2) reduction(+:sum)
  for (i = 1; i <= LOOPCOUNT; i++)
    {
      sum = sum + (i + i2);
    }				/*end of for */


  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2 + 3 * LOOPCOUNT;
  return (known_sum == sum);
}				/* end of check_parallel_for_fistprivate */

int
crosscheck_parallel_for_firstprivate (FILE * logFile)
{
  int sum = 0;
  /*int sum0=0; */
  int known_sum;
  int i2 = 3;

  int i;
#pragma omp parallel for private(i2) reduction(+:sum)
  for (i = 1; i <= LOOPCOUNT; i++)
    {
      sum = sum + (i + i2);
    }				/*end of for */


  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2 + 3 * 999;
  return (known_sum == sum);
}				/* end of check_parallel_for_fistprivate */
