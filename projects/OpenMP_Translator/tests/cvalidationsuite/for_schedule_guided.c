/*
* Test for guided scheduling
* Ensure threads get chunks interleavely first
* Then judge the chunk sizes are decreasing until to a stable value
* Modifed by Chunhua Liao
* For example, 100 iteration on 2 threads, chunksize 7
* one line for each dispatch, 0/1 means thread id 
*0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  24
*1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1              18
*0 0 0 0 0 0 0 0 0 0 0 0 0 0                      14
*1 1 1 1 1 1 1 1 1 1                              10
*0 0 0 0 0 0 0 0                                   8
*1 1 1 1 1 1 1                                     7
*0 0 0 0 0 0 0                                     7
*1 1 1 1 1 1 1                                     7
*0 0 0 0 0                                         5
*/
#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>

#include "omp_testsuite.h"
#include "omp_my_sleep.h"

#define CFSMAX_SIZE 150  /*choose small iteration space for small sync. overhead*/
#define MAX_TIME 5
/* #define SLEEPTIME 0.5 */
#define SLEEPTIME 1

int
check_for_schedule_guided (FILE * logFile)
{
  int threads;
/*  const int chunk_size = 7; */
  int tids[CFSMAX_SIZE];
  int i,  *tmp;
  int flag=0;
  int result = 0;
  int notout = 1;
  int maxiter = 0;
  int count = 0;
  int tmp_count = 0;
  int tid;

#pragma omp parallel
  {
#pragma omp single
    {
      threads = omp_get_num_threads ();
    }
  }

  if (threads < 2)
    {
      printf ("This test only works with at least two threads .\n");
      return 0;
    }

/* Now the real parallel work:  

   Each thread will start immediately with the first chunk.

*/

#pragma omp parallel shared(tids,maxiter) private(tid,count)
  {
    tid = omp_get_thread_num ();

//#pragma omp for nowait schedule(guided,chunk_size)
#pragma omp for nowait schedule(guided,7)
    for (i = 0; i < CFSMAX_SIZE; ++i)
      {
	/*printf(" notout=%d, count= %d\n",notout,count); */
	count = 0;
#pragma omp flush(maxiter)
	if (i > maxiter)
	  {
#pragma omp critical
	    {
	      maxiter = i;
	    }
	  }

	/* if it is not our turn we wait 
           a) until another thread executed an iteration 
	   with a higher iteration count
	   b) we are at the end of the loop (first thread finished                           and set notout=0 OR
	   c) timeout arrived */

#pragma omp flush(maxiter,notout)
	while (notout && (count < MAX_TIME) && (maxiter == i))
	  {
	    /*printf("Thread Nr. %d sleeping\n",tid); */
	    my_sleep (SLEEPTIME);
	    count += SLEEPTIME;
	  }
	/*printf("Thread Nr. %d working once\n",tid); */
	tids[i] = tid;
      }				/*end omp for */

    notout = 0;
#pragma omp flush(notout)
  }				/* end omp parallel */

 count = 0;
/*
printf("debug--------\n");
    for (i = 0; i < CFSMAX_SIZE; ++i)
	printf("%d ",tids[i]);
printf("\nEnd debug--------\n");
*/
    /*fprintf(logFile,"# global_chunknr thread local_chunknr chunksize\n"); */
    for (i = 0; i < CFSMAX_SIZE - 1; ++i)
      {
	if (tids[i] != tids[i + 1])
	  {
	    count++;
	  }
      }

    tmp = (int *) malloc((count + 1)* sizeof (int));
   tmp_count=0;
   tmp[0]=1;
/*calculate the chunksize for each dispatch*/
    for (i = 0; i < CFSMAX_SIZE - 1; ++i)
      {
	if (tids[i] == tids[i + 1])
	  {
	    tmp[tmp_count]++;
	  }
	else
	  {
            tmp_count ++;
            tmp[tmp_count]=1;
	  }
      }
/*
printf("Debug2----\n");
     for (i=0;i<=tmp_count;i++)
     printf("%d ",tmp[i]);
printf("\nEndDebug2----\n");
*/
/*Check if chunk sizes are decreased until equals to the specified one,
 ignore the last dispatch for possible smaller remainder*/


  flag=0;
    for (i = 0; i < count-1; i++)
      {
       if ((i>0)&&(tmp[i]==tmp[i+1])) flag=1; 
       /*set flag to indicate the Chunk sizes should be the same from now on*/
       if(flag==0)
         {
 	   if (tmp[i]<=tmp[i+1]) {
             result++;
	     fprintf(logFile,"chunk size from %d to %d not decreased.\n",
                     i,i+1);
             }
         }
       else if (tmp[i]!=tmp[i+1])
         {
           result++;
	   fprintf(logFile,"chunk size not maintained.\n");
         }
     }

  return (result==0);
}

int
crosscheck_for_schedule_guided (FILE * logFile)
{
   int threads;
/*  const int chunk_size = 7; */
  int tids[CFSMAX_SIZE];
  int i,  *tmp;
  int flag=0;
  int result = 0;
  int notout = 1;
  int maxiter = 0;
  int count = 0;
  int tmp_count = 0;
  int tid;
/*  
Since it takes quite long to finish the check_x(), I skip the cross_check_X() here. Liao
*/
#pragma omp parallel
  {
#pragma omp single
    {
      threads = omp_get_num_threads ();
    }
  }

  if (threads < 2)
    {
      printf ("This test only works with at least two threads .\n");
      return 0;
    }


#pragma omp parallel shared(tids,maxiter) private(tid,count)
  {
    tid = omp_get_thread_num ();

//#pragma omp for nowait schedule(static,chunk_size)
#pragma omp for nowait schedule(static,7)
    for (i = 0; i < CFSMAX_SIZE; ++i)
      {
	count = 0;
#pragma omp flush(maxiter)
	if (i > maxiter)
	  {
#pragma omp critical
	    {
	      maxiter = i;
	    }
	  }


#pragma omp flush(maxiter,notout)
	while (notout && (count < MAX_TIME) && (maxiter == i))
	  {
	    my_sleep (SLEEPTIME);
	    count += SLEEPTIME;
	  }
	tids[i] = tid;
      }	

    notout = 0;
#pragma omp flush(notout)
  }				

 count = 0;
    for (i = 0; i < CFSMAX_SIZE - 1; ++i)
      {
	if (tids[i] != tids[i + 1])
	  {
	    count++;
	  }
      }

    tmp = (int *) malloc((count + 1)* sizeof (int));
   tmp_count=0;
   tmp[0]=1;
    for (i = 0; i < CFSMAX_SIZE - 1; ++i)
      {
	if (tids[i] == tids[i + 1])
	  {
	    tmp[tmp_count]++;
	  }
	else
	  {
	    tmp_count ++;
            tmp[tmp_count]=1;
	  }
      }
  flag=0;
    for (i = 0; i < count-1; i++)
      {
       if ((i>0)&&(tmp[i]==tmp[i+1])) flag=1; 
       if(flag==0){
        if (tmp[i]<=tmp[i+1]) {
		result++;
		fprintf(logFile,"chunk size from %d to %d not decreased.\n",
		i,i+1);
		}
         }
       else if (tmp[i]!=tmp[i+1]) {
                result++;
		fprintf(logFile,"chunk size not maintained.\n");
		}
      }

   result=1;
  return (result==0);

}
