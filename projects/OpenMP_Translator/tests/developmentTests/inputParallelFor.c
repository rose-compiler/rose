#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

static long num_steps=10000000;
double step;

int main()
{
  int i;
  double x,pi, sum=0.0;
  step=1.0/(double)num_steps;

#pragma omp parallel for reduction (+:sum) private (x)
for(i=1;i<=num_steps;i++)
{
	x=(i-0.5)*step;
       	sum=sum+ 4.0/(1.0+x*x);
}
 pi=step*sum;
 printf("step:%e sum:%f PI=%.20f\n",step,sum, pi);

 return 0;
}

