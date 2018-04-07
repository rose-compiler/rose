#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <assert.h>

static int sum0=0;
#pragma omp threadprivate(sum0)

int main()
{
  int sum=0,sum1=0;
  int i;
#pragma omp parallel
  {
    sum0=0;
#pragma omp for
    for (i=1;i<=1000;i++)
    {
      sum0=sum0+i;
    }                       /*end of for*/
#pragma omp critical
    {
      printf("partial sum0 is:%d\n",sum0);
      sum= sum+sum0;
    }    /*end of critical*/
  }           /* end of parallel*/

  for (i=1;i<=1000;i++)
  {
    sum1=sum1+i;
  }                       /*end of for*/
  printf("sum=%d; sum1=%d\n",sum,sum1);
  assert(sum==sum1);
  return 0;
}

