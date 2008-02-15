#include <stdio.h>
#include <math.h>
#include "omp_testsuite.h"

/* Utility function do spend some time in a loop*/
static void
do_some_work ()
{
  int i;
  double sum = 0;
  for (i = 0; i < 1000; i++)
    {
      sum += sqrt (i);
    }
}

int
check_for_private (FILE * logFile)
{
  int sum = 0;
  int sum0 = 0;
  int sum1 = 0;
  int known_sum;
  int i;
#pragma omp parallel private(sum1)
  {
    sum0 = 0;
    sum1 = 0;
#pragma omp for private(sum0) schedule(static,1)
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum0 = sum1;
#pragma omp flush
	sum0 = sum0 + i;
	do_some_work ();
#pragma omp flush
	sum1 = sum0;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum1;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
  return (known_sum == sum);
}				/* end of check_for_private */

int
crosscheck_for_private (FILE * logFile)
{
  int sum = 0;
  int sum0 = 0;
  int sum1 = 0;
  int known_sum;
  int i;
#pragma omp parallel private(sum1)
  {
    sum0 = 0;
    sum1 = 0;
#pragma omp for schedule(static,1)
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum0 = sum1;
#pragma omp flush
	sum0 = sum0 + i;
	do_some_work ();
#pragma omp flush
	sum1 = sum0;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum1;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
  return (known_sum == sum);
}				/* end of check_for_private */
