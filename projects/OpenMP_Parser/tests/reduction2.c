#include <stdio.h>
#include <omp.h>

#define NUM_THREADS 4

void main ()
{
 int i,total=1000000;
 double res=0.0;

 omp_set_num_threads(NUM_THREADS);

 #pragma omp parallel for reduction(+:res) 
 for (i=0; i<= total; i++){
  res = res + i;
  res = res - 2*i;
  }
 
 printf("the sum of 1000000 is :%.0f\n", res);
}
