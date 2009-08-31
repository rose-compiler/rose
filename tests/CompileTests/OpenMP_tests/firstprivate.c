#include <stdio.h>
#include <omp.h>

int main()
{
  int i=100,sum=0;

#pragma omp parallel firstprivate (i) reduction(+:sum)
  {
    sum=sum + i;
  }

  return 1;
}

