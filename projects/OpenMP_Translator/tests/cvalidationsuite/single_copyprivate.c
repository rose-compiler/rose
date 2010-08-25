
#include <omp.h>
#include "omp_testsuite.h"

int
check_single_copyprivate (FILE * logFile)
{
  int result = 0;
  int nr_iterations = 0;
  int i;
  int j;
#pragma omp parallel private(i,j)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
	/*
           int thread;
	   thread=omp_get_thread_num();
	 */
#pragma omp single copyprivate(j)
	{
	  nr_iterations++;
	  j = i;
	  /*printf("thread %d assigns ,j=%d,i=%d\n",thread,j,i); */
	}
	/*      #pragma omp barrier */
#pragma omp critical
	{
	  /*printf("thread=%d,j=%d,i=%d\n",thread,j,i); */
	  result = result + j - i;
	}
#pragma omp barrier
      }				/* end of for */

  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}



int
crosscheck_single_copyprivate (FILE * logFile)
{
  int result = 0;
  int nr_iterations = 0;
  int i;
  int j;
#pragma omp parallel private(i,j)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
	/*
           int thread;
	   thread=omp_get_thread_num();
	 */
#pragma omp single 
	{
	  nr_iterations++;
	  j = i;
	  /*printf("thread %d assigns ,j=%d,i=%d\n",thread,j,i); */
	}
	/*      #pragma omp barrier */
#pragma omp critical
	{
	  /*printf("thread=%d,j=%d,i=%d\n",thread,j,i); */
	  result = result + j - i;
	}
#pragma omp barrier
      }				/* end of for */

  }				/* end of parallel */
  return (result == 0) && (nr_iterations == LOOPCOUNT);
}
