#include <stdio.h>
#include <omp.h>
#include <assert.h>

int main()
{
  int i,sum=77;
  int num_steps=100;

  omp_set_num_threads(4);

#pragma omp parallel for firstprivate (sum) lastprivate (sum)
  for(i=1;i<=num_steps;i++)
  {
    sum=sum + i;
  }
  printf("sum=%d\n",sum);

  return 0;
}

