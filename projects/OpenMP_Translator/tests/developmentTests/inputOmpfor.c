/*************************************************
PI calculation

  separated omp for, with different scheduling policies

The result will be slightly different from one run to another
if dynamic/guided scheduling is used
since the different orders of floating point operations

By C.Liao
**************************************************/
#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

static long num_steps = 10000000;
double step;

int k_3 = 100;
//  int k_4=100;
int
main ()
{
  double x, pi, sum = 0.0;
  int i;
  step = 1.0 / (double) num_steps;
  int chunksize=100;

#pragma omp parallel private (x)
  {
#pragma omp single
    printf ("Running using %d threads..\n", omp_get_num_threads ());

//#pragma omp for reduction(+:sum)
//#pragma omp for reduction(+:sum) schedule(runtime)
#pragma omp for reduction(+:sum) schedule(static,chunksize)
    for (i = 1; i <= num_steps; i++)
      {
	k_3++;
	x = (i - 0.5) * step;
	sum = sum + 4.0 / (1.0 + x * x);
      }
  }
  pi = step * sum;
  printf ("step:%e sum:%f PI=%.20f\n", step, sum, pi);

  return 0;
}
