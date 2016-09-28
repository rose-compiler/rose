#include <stdio.h>
#if defined(_OPENMP)
#include <omp.h>
#endif

int main(void)
{
  int i;
#pragma omp parallel for ordered schedule (static,5)
  for (i=0;i<20;i++)
  {
#pragma omp ordered 
    printf("%2d,Hello,world.!\n",i);
  }
  return 0;
}
