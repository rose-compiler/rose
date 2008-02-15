/*************************************************
orphaned 'omp for' (in a function separated from its caller 'omp parallel')
**************************************************/
#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

void foo(void);
static long num_steps=10000000;
double step;

int main()
{
  step=1.0/(double)num_steps;

#pragma omp parallel 
{
  foo();
}

 return 0;
}

void foo (void)
{
  int i;
  double x,pi;
  double sum=0.0;
  #pragma omp single
  printf("Running using %d threads..\n", omp_get_num_threads());

#pragma omp for reduction(+:sum) schedule(static)
  for(i=1;i<=num_steps;i++)
  {
	x=(i-0.5)*step;
       	sum=sum+ 4.0/(1.0+x*x);
  }

#pragma omp single
  {
    pi=step*sum;
    printf("step:%e sum:%f PI=%.20f\n",step,sum, pi);
  }
}
