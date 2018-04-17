
#include <omp.h>
#include <stdio.h>
#define LOOPCOUNT 200
int
check_single_copyprivate (FILE * logFile)
{
  int result = 0;
  int nr_iterations = 0;
  int i;
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
	int j;
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
#pragma omp parallel private(i)
  {
    for (i = 0; i < LOOPCOUNT; i++)
      {
	int j;
        /*
           int thread;
           thread=omp_get_thread_num();
         */
#pragma omp single private(j)
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
