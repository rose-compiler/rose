#include <stdio.h>
#include "omp_testsuite.h"

int
check_single_private (FILE * logFile)
{
  int nr_threads_in_single = 0;
  int result = 0;
  int myresult = 0;
  int myit = 0;
  int nr_iterations = 0;
  int i;
#pragma omp parallel private(i,myresult,myit)
  {
    myresult = 0;
    myit = 0;
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single private(nr_threads_in_single) nowait
	{
	  nr_threads_in_single = 0;
#pragma omp flush
	  nr_threads_in_single++;
#pragma omp flush
	  myit++;
/*	  nr_threads_in_single--; */
	  myresult = myresult + nr_threads_in_single;
	}			/* end of single */
      }				/* end of for  */
#pragma omp critical
    {
/*      result += myresult; */
      result += nr_threads_in_single;
      nr_iterations += myit;
    }
  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}				/* end of check_single private */

int
crosscheck_single_private (FILE * logFile)
{
  int nr_threads_in_single = 0;
  int result = 0;
  int myresult = 0;
  int myit = 0;
  int nr_iterations = 0;
  int i;
#pragma omp parallel private(i,myresult,myit)
  {
    myresult = 0;
    myit = 0;
    for (i = 0; i < LOOPCOUNT; i++)
      {
#pragma omp single nowait
	{
	  nr_threads_in_single = 0;
#pragma omp flush
	  nr_threads_in_single++;
#pragma omp flush
	  myit++;
/*	  nr_threads_in_single--; */
	  myresult = myresult + nr_threads_in_single;
	}			/* end of single */
      }				/* end of for  */
#pragma omp critical
    {
      result += nr_threads_in_single;
      nr_iterations += myit;
    }
  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}				/* end of check_single private */
