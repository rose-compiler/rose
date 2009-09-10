#include <stdio.h>
#include <omp.h>
static long num_steps = 100000; 
double step;
#define NUM_THREADS 20

int main ()
{	
  int i;
  int id;
  double x, pi, sum[NUM_THREADS];
  step = 1.0/(double) num_steps;
  omp_set_num_threads(NUM_THREADS);
#pragma omp parallel 
  {
    id = omp_get_thread_num();
    for (i=id, sum[id]=0.0;i< num_steps; i=i+NUM_THREADS){
      x = (i+0.5)*step;
      sum[id] += 4.0/(1.0+x*x);
    }
  }
  for(i=0, pi=0.0;i<NUM_THREADS;i++)pi += sum[i] * step;
  printf("PI=%f\n",pi);
  return 0;
}

