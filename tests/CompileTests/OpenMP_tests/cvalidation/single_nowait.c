#include <stdio.h>
#include "omp_testsuite.h"


int
check_single_nowait (FILE * logFile)
{
  int nr_iterations = 0;
  int total_iterations = 0;
  int my_iterations = 0;
  int i;
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single nowait
	{
#pragma omp atomic
	  nr_iterations++;
	}			/* end of single */
      }				/* end of for  */
  }				/* end of parallel */

#pragma omp parallel private(i,my_iterations)
  {
    my_iterations = 0;
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single nowait
	{
	  my_iterations++;
	}			/* end of single */
      }				/* end of for  */
#pragma omp critical
    {
      total_iterations += my_iterations;
    }

  }				/* end of parallel */
  return ((nr_iterations == LOOPCOUNT) && (total_iterations == LOOPCOUNT));
}				/* end of check_single_nowait */


int
crosscheck_single_nowait (FILE * logFile)
{
  int nr_iterations = 0;
  int total_iterations = 0;
  int my_iterations = 0;
  int i;
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {

	{
#pragma omp atomic
	  nr_iterations++;
	}			/* end of single */
      }				/* end of for  */
  }				/* end of parallel */

#pragma omp parallel private(i,my_iterations)
  {
    my_iterations = 0;
    for (i = 0; i < LOOPCOUNT; i++)
      {

	{
	  my_iterations++;
	}			/* end of single */
      }				/* end of for  */
#pragma omp critical
    {
      total_iterations += my_iterations;
    }

  }				/* end of parallel */
  return ((nr_iterations == LOOPCOUNT) && (total_iterations == LOOPCOUNT));
}				/* end of check_single_nowait */
