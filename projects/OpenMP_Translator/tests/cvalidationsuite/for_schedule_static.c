/*
*
* For static scheduling we check whether the chunks have the requested size,
* with the legal exception of the last chunk. 
* Modified by Chunhua Liao
*/
#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>

#include "omp_testsuite.h"
#include "omp_my_sleep.h"

#define CFSMAX_SIZE 1000

/****************************************************************/

int
check_for_schedule_static (FILE * logFile)
{
  const int chunk_size = 7;
  int threads;
  int tids[CFSMAX_SIZE];
  int i;
  int result = 0;
  int ii = 0;
  int tid;

#pragma omp parallel 
  {				/* begin of parallel */
#pragma omp single
    {
      threads = omp_get_num_threads ();
    }
  }				/* end of parallel */

  if (threads < 2)
    {
      printf ("This test only works with at least two threads");
      fprintf (logFile, "This test only works with at least two threads");
      return 0;
    }
  else
    {
      fprintf (logFile,
       "Using an internal count of %d\nUsing a specified chunksize of %d\n",
       CFSMAX_SIZE, chunk_size);

#pragma omp parallel shared(tids) private(tid)
      {				/* begin of parallel */
	tid = omp_get_thread_num ();

#pragma omp for schedule(static,chunk_size)
	for (i = 0; i < CFSMAX_SIZE; i++)
	  {
	    tids[i] = tid;
	  }
      }				/* end of parallel */
/*
printf("debug---------------------\n");
for (i=0;i<CFSMAX_SIZE -1;i++)
  printf("%d ",tids[i]);
printf("End of debug---------------------\n");
*/
      for (i = 0; i < CFSMAX_SIZE; ++i)
	{
	ii= (i/chunk_size) % threads; /*round-robin for static chunk*/
	  if (tids[i] != ii)
	    {
              result++;
              fprintf (logFile,
                       "Iteration %d should be assigned to %d instead of %d\n",
                        i,ii,tids[i]);
	    }
	}
      /*printf("Alles OK beim Test von schedule(static)\n"); */
      return (result==0);
    }
}

/****************************************************************/
int
crosscheck_for_schedule_static (FILE * logFile)
{
  const int chunk_size = 7;
  int threads;
  int tids[CFSMAX_SIZE];
  int i;
  int result = 0;
  int ii = 0;
  int tid;

#pragma omp parallel 
  {				/* begin of parallel */
#pragma omp single
    {
      threads = omp_get_num_threads ();
    }
  }				/* end of parallel */

  if (threads < 2)
    {
      printf ("This test only works with at least two threads");
      fprintf (logFile, "This test only works with at least two threads");
      return 0;
    }
  else
    {
      fprintf (logFile,
       "Using an internal count of %d\nUsing a specified chunksize of %d\n",
       CFSMAX_SIZE, chunk_size);

#pragma omp parallel shared(tids) private(tid)
      {				/* begin of parallel */
	tid = omp_get_thread_num ();

#pragma omp for 
	for (i = 0; i < CFSMAX_SIZE; i++)
	  {
	    tids[i] = tid;
	  }
      }				/* end of parallel */
/*
printf("debug---------------------\n");
for (i=0;i<CFSMAX_SIZE -1;i++)
  printf("%d ",tids[i]);
printf("End of debug---------------------\n");
*/
      for (i = 0; i < CFSMAX_SIZE; ++i)
	{
	ii= (i/chunk_size) % threads; /*round-robin for static chunk*/
	  if (tids[i] != ii)
	    {
              result++;
	      fprintf (logFile,
                       "Iteration %d should be assigned to %d instead of %d\n",
                        i,ii,tids[i]);
	    }
	}
      /*printf("Alles OK beim Test von schedule(static)\n"); */
      return (result==0);
    }

}

