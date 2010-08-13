/*
*
* threadprivate is tested in 2 ways
* 1. The global variable declared as threadprivate should have local copy 
*    for each  thread.  Otherwise race condition for wrong result
* 2. if the value of local copy is retained for two adjacent parallel region
*/
#include "omp_testsuite.h"
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

static int sum0 = 0;
#pragma omp threadprivate(sum0)

static int myvalue = 0;
#pragma omp threadprivate(myvalue)

int
check_omp_threadprivate (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;

  int iter;

  int *data;
  int size;
  int failed = 0;
  int my_random;
  omp_set_dynamic (0);

#pragma omp parallel
  {
    sum0 = 0;
#pragma omp for
    for (i = 1; i <= LOOPCOUNT; i++)
      {
	sum0 = sum0 + i;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum0;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;
  if (known_sum != sum)
    {
      fprintf (logFile, " known_sum = %d , sum = %d \n", known_sum, sum);
    }


  /* the next parallel region is just used to get the number of threads */
  omp_set_dynamic (0);
#pragma omp parallel
  {
#pragma omp master
    {
      size = omp_get_num_threads ();
      data = (int *) malloc (size * sizeof (int));
    }
  }				/* end parallel */


  srand (45);
  for (iter = 0; iter < 100; iter++)
    {
      my_random = rand ();	/* random number generator is called inside serial region */

      /* the first parallel region is used to initialiye myvalue and the array with my_random+rank */
#pragma omp parallel
      {
	int rank;
	rank = omp_get_thread_num ();
	myvalue = data[rank] = my_random + rank;
      }

      /* the second parallel region verifies that the value of "myvalue" is retained */
#pragma omp parallel reduction(+:failed)
      {
	int rank;
	rank = omp_get_thread_num ();
	failed = failed + (myvalue != data[rank]);
	if (myvalue != data[rank])
	  {
	    fprintf (logFile, " myvalue = %d, data[rank]= %d\n", myvalue,
                     data[rank]);
          }
      }
    }
  free (data);

  return (known_sum == sum) && !failed;

}				/* end of check_threadprivate */




static int crosssum0 = 0;
/*#pragma omp threadprivate(crosssum0)*/

static int crossmyvalue = 0;
/*#pragma omp threadprivate(crossmyvalue)*/

int
crosscheck_omp_threadprivate (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;

  int iter;

  int *data;
  int size;
  int failed = 0;
  int my_random;
  omp_set_dynamic (0);

#pragma omp parallel
  {
    crosssum0 = 0;
#pragma omp for
    for (i = 1; i < LOOPCOUNT; i++)
      {
	crosssum0 = crosssum0 + i;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + crosssum0;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (LOOPCOUNT * (LOOPCOUNT + 1)) / 2;



#pragma omp parallel
  {
#pragma omp master
    {
      size = omp_get_num_threads ();
      data = (int *) malloc (size * sizeof (int));
    }
  }
  srand (45);
  for (iter = 0; iter < 100; iter++)
    {
      my_random = rand ();
#pragma omp parallel
      {
	int rank;
	rank = omp_get_thread_num ();
	crossmyvalue = data[rank] = my_random + rank;
      }
#pragma omp parallel reduction(+:failed)
      {
	int rank;
	rank = omp_get_thread_num ();
	failed = failed + (crossmyvalue != data[rank]);
      }
    }
  free (data);

  return (known_sum == sum) && !failed;

}				/* end of crosscheck_threadprivate */
