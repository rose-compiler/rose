/*
* test #if #else #endif
* from NPB 2.3 C version LU
*  #if defined(_OPENMP) is attached to the for loop,
*  which will be moved under SgOmpForStatement as its body.
*
*  In this case, we want to #if defined(_OPENMP) to go 
*  with the for loop and not be automatically kept in its original place
*  by SageInterface::removeStatement();
*
* Liao 10/28/2010
*/
#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif 
int a[20];
int main(void)
{
  int i;
#pragma omp parallel
  {
#pragma omp single
    printf ("Using %d threads.\n",omp_get_num_threads());
#pragma omp for schedule(static)
#if defined(_OPENMP)     
    for (i=0;i<20;i+=1)
#else      
    for (i=19;i>=0;i-=1)
#endif      
    {
      a[i]=i*2;
      printf("Iteration %2d is carried out by thread %2d\n",\
          i, omp_get_thread_num());
    }
  }
}

