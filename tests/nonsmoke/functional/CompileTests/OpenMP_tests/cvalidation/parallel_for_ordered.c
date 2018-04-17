// Skip testing on 64 bit systems for now!
#ifndef __LP64__

#include <stdio.h>
#include <math.h>
#include "omp_testsuite.h"

static int last_i = 0;

/*! 
  Utility function: returns true if the passed argument is larger than 
  the argument of the last call of this function.
  */
static int
check_i_islarger2 (int i)
{
  int islarger;
  islarger = (i > last_i);
  last_i = i;
  return (islarger);
}

int
check_parallel_for_ordered (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;
  int is_larger = 1;
  last_i = 0;
#pragma omp parallel for schedule(static,1) ordered
  for (i = 1; i < 100; i++)
    {
#pragma omp ordered
      {
	is_larger = check_i_islarger2 (i) && is_larger;
	sum = sum + i;
      }
    }
  known_sum = (99 * 100) / 2;
  return (known_sum == sum) && is_larger;
}

int
crosscheck_parallel_for_ordered (FILE * logFile)
{
  int sum = 0;
  int known_sum;
  int i;
  int is_larger = 1;
  last_i = 0;
#pragma omp parallel for schedule(static,1)
  for (i = 1; i < 100; i++)
    {

      {
	is_larger = check_i_islarger2 (i) && is_larger;
	sum = sum + i;
      }
    }
  known_sum = (99 * 100) / 2;
  return (known_sum == sum) && is_larger;
}

#else
  #warning "Not tested on 64 bit systems"
#endif

