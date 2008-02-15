#include <stdio.h>
#include "omp_testsuite.h"


int
check_single (FILE * logFile)
{
  int nr_threads_in_single = 0;
  int result = 0;
  int nr_iterations = 0;
  int i;
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single
	{
#pragma omp flush
	  nr_threads_in_single++;
#pragma omp flush
	  nr_iterations++;
	  nr_threads_in_single--;
	  result = result + nr_threads_in_single;
	}			/* end of single */
      }				/* end of for  */
  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}				/* end of check_single */

int
crosscheck_single (FILE * logFile)
{
  int nr_threads_in_single = 0;
  int result = 0;
  int nr_iterations = 0;
  int i;
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {

	{
#pragma omp flush
	  nr_threads_in_single++;
#pragma omp flush
	  nr_iterations++;
	  nr_threads_in_single--;
	  result = result + nr_threads_in_single;
	}			/* end of single */
      }				/* end of for  */
  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}				/* end of check_single */
