/*
 *  Test the insertion of multiple calls to runtime terminate functions.
 *  The function calls should not share the same statement.
 *  7/25/2011
By C. Liao
*/
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(void)
{
  int i=0, j=0;
#pragma omp parallel default(shared) private(i)
  {
#ifdef _OPENMP
    i=omp_get_thread_num()+j;
#endif
    printf("Hello,world! I am thread %d\n",i);
  }

  if (i)
    return 0;
  else
    return 0;
}

