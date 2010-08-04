#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int counter=0;

#ifdef _OPENMP
#pragma omp threadprivate(counter)
#endif 
int main(void)
{
 int i;
#pragma omp parallel for
 for(i=0;i<10000;i++)
    counter++;
#pragma omp parallel for
 for(i=0;i<10000;i++)
    counter+=3;
#pragma omp parallel
  printf("counter=%d\n",counter);
}
