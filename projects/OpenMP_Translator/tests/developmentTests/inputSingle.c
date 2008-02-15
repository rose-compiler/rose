// test case for single
//It contains private(), firstprivate(), nowait
//TODO copyprivate()

#include <stdio.h>
#ifdef _OPENMP
#include "omp.h"
#endif

int y;
int main()
{
int x;
int z=100;
#pragma omp parallel 
{
  #pragma omp single private(x,y) firstprivate(z) nowait
  {
   x = omp_get_thread_num(); 
   y = omp_get_num_threads()+z;
  printf("I am thread %d out of %d threads\n", \
	x, y); 
  }

}
return 0;
}
