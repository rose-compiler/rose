#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(void)
{
int i=0;
#pragma omp parallel 
{
#ifdef _OPENMP
 i=omp_get_max_threads();
#endif
 printf("Hello,world! I am thread %d\n",i);
}
return 0;
}

