#include <omp.h>
#include <assert.h>
int main()
{
  int sum=7;
  int known_sum;
  int i;
#pragma omp parallel sections reduction(+:sum) private(i) if(1)
  {
#pragma omp section
    {
      for (i=1;i<400;i++){
        sum += i;
      }
    }
#pragma omp section
    {
      for(i=400;i<700;i++)
        sum += i;
    }
#pragma omp section
    {
      for(i=700;i<1000;i++)
        sum += i;
    }

  }/* end of section reduction.*/

  known_sum=(999*1000)/2+7;
  assert (known_sum==sum);
  return 0;
}

