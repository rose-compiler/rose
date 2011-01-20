/*
1. A local copy of reduction variable  is made and initialized depending on the op(e.g. 0 for +).
2. Compiler finds standard reduction expressions containing op and uses them to update the local copy. 
3. Local copies are reduced into a single value and combined with the original global value.

*/
#include <stdio.h>
#include <assert.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main()
{
  int i;
  long sum=0;
  int upper=100;

#pragma omp parallel for reduction(+:sum)
  for (i=1; i<= upper; i++){
    sum = sum + i;
  }
  printf("sum from 1 to %d is %d\n", upper, sum);
  assert (sum==5050);
  return 0;
}

