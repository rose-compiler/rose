/*
1. A local copy of reduction variable  is made and initialized depending on the op(e.g. 0 for +).
2. Compiler finds standard reduction expressions containing op and uses them to update the local copy. 
3. Local copies are reduced into a single value and combined with the original global value.

*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int i;
  long sum=0;
  int total=100;

#pragma omp parallel for reduction(+:sum)
  for (i=0; i<= total; i++){
    sum = sum + i;
  }

  long sum0;
#pragma omp parallel private(sum0)
  {
    sum0=0; 

#pragma omp for private(i)
    for (i=0; i<= total; i++)
      sum0=sum0+i;

#pragma omp critical
    sum = sum + sum0; 
  }
  printf("sum of 1 to %d = %ld\n",total,sum);

  return 0;
}

