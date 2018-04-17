// Task dependencies are part of OpenMP 4.0, which is only supported in GCC 4.9 or newer.
#include <stdio.h>
int main()
{
  int x = 1;
#pragma omp parallel
#pragma omp single
  {
#pragma omp task shared(x) depend(out: x)
    x = 2;

#pragma omp task shared(x) depend(in: x)
    printf("x = %d\n", x);
  }
  return 0;
}
