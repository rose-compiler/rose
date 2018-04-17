/*
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

void foo(int * i)
{
  *i =2;
}

int main(void)
{
  int i=0;
#pragma omp parallel default(shared) private(i)
  {
#ifdef _OPENMP
    i=omp_get_thread_num();
#endif
    foo (&i);
    printf("Hello,world! I am thread %d\n",i);
    i++;
  }
  return 0;
}

