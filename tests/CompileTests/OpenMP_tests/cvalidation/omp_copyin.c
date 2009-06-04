
// Skip testing on 64 bit systems for now!
#ifndef __LP64__

#include "omp_testsuite.h"
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
/*
static int sum0 = 0;
#pragma omp threadprivate(sum0)

static int myvalue = 0;
#pragma omp threadprivate(myvalue)
*/

static int sum1 = 789;
#pragma omp threadprivate(sum1)

int
check_omp_copyin (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;
  sum1 = 0;
#pragma omp parallel copyin(sum1)
  {
    /*printf("sum1=%d\n",sum1); */
#pragma omp for
    for (i = 1; i < 1000; i++)
      {
	sum1 = sum1 + i;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + sum1;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2;
  return (known_sum == sum);

}				/* end of check_threadprivate */

static int crosssum1 = 789;
#pragma omp threadprivate(crosssum1)

int
crosscheck_omp_copyin (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;
  crosssum1 = 0;
#pragma omp parallel
  {
    /*printf("sum1=%d\n",sum1); */
#pragma omp for
    for (i = 1; i < 1000; i++)
      {
	crosssum1 = crosssum1 + i;
      }				/*end of for */
#pragma omp critical
    {
      sum = sum + crosssum1;
    }				/*end of critical */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2;
  return (known_sum == sum);

}				/* end of check_threadprivate */

static int myvalue2 = 0;


int
crosscheck_spmd_threadprivate (FILE * logFile)
{
  int iter;

  int *data;
  int size;
  int failed = 0;
  int my_random;
  omp_set_dynamic (0);

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
	myvalue2 = data[rank] = my_random + rank;
      }
#pragma omp parallel reduction(+:failed)
      {
	int rank;
	rank = omp_get_thread_num ();
	failed = failed + (myvalue2 != data[rank]);
      }
    }
  free (data);
  return !failed;
}

#else
  #warning "Not tested on 64 bit systems"
#endif

