/* This file contains all checks for the section construct without the checks for the reduction clauses:

   ordered: checks that the execution is equivalent to the serial case


 */

// Skip testing on 64 bit systems for now!
#ifndef __LP64__

#include <stdio.h>
#include "omp_testsuite.h"


int
check_parallel_section_firstprivate (FILE * logFile)
{
  int sum = 7;
  int sum0 = 11;
  int known_sum;
#pragma omp parallel sections firstprivate(sum0)
  {
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
  }				/*end of parallel sections */
  known_sum = 11 * 3 + 7;
  return (known_sum == sum);
}				/* end of check_section_firstprivate */


int
crosscheck_parallel_section_firstprivate (FILE * logFile)
{
  int sum = 7;
  int sum0 = 11;
  int known_sum;
#pragma omp parallel sections private(sum0)
  {
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
#pragma omp section
    {
#pragma omp critical
      {
	sum = sum + sum0;
      }				/*end of critical */
    }
  }				/*end of parallel sections */
  known_sum = 11 * 3 + 7;
  return (known_sum == sum);
}				/* end of check_section_firstprivate */

#else
  #warning "Not tested on 64 bit systems"
#endif

