#include <stdio.h>
#include <math.h>
#include "omp_testsuite.h"
#include <omp.h>

int
check_for_firstprivate (FILE * logFile)
{
  int sum = 0;
  int sum0 = 12345; /*bug 162, Liao*/
  int sum1 = 0;
  int known_sum;
  int threadsnum;
  int i;
#pragma omp parallel firstprivate(sum1)
  {
#pragma omp single
  {
   threadsnum=omp_get_num_threads(); 
   }
    /*sum0=0; */
#pragma omp for firstprivate(sum0)
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum0 = sum0 + i;
	sum1 = sum0;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum1;
    }				/*end of critical */
  }				/* end of parallel */
/* bug 162 , Liao*/
  known_sum = 12345* threadsnum+ (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
  return (known_sum == sum);
  /*return (known_sum == sum); */
}				/* end of check_for_fistprivate */

int
crosscheck_for_firstprivate (FILE * logFile)
{
  int sum = 0;
  int sum0 = 12345;
  int sum1 = 0;
  int known_sum;
  int threadsnum;
  int i;
#pragma omp parallel
  {
   threadsnum=omp_get_num_threads();
   }

#pragma omp parallel firstprivate(sum1)
  {
    /*sum0=0; */
#pragma omp for private(sum0)
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum0 = sum0 + i;
	sum1 = sum0;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum1;
    }				/*end of critical */
  }				/* end of parallel */
 known_sum = 12345* threadsnum+ (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
  return (known_sum == sum);
}				/* end of check_for_fistprivate */
