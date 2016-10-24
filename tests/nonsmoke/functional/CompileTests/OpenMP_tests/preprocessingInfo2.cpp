/*
test preprocessing info before and after a statement
Also the insertion of omp.h when necessary 
*/
#include <omp.h>
int main()
{
int i;
#pragma omp parallel
  {
#if defined(_OPENMP)
  #pragma omp master
    {
	i++;
    }
#endif
  }
  return 0;
}

