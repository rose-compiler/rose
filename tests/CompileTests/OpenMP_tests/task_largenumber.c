#include <stdio.h>
#include <omp.h>

#define LARGE_NUMBER 10
//#define LARGE_NUMBER 10000000
double item[LARGE_NUMBER];
void process (double input)
{
  printf("processing %f by thread %d\n",input, omp_get_thread_num());
}
int cutoff =1000;
int
main ()
{
#pragma omp parallel
  {
#pragma omp single
    {
      int i;
      printf("Using %d threads.\n",omp_get_num_threads());
      for (i = 0; i < LARGE_NUMBER; i++)
        #pragma omp task if (i < cutoff)
         process (item[i]);
    }
  }
  return 0;
}
