#include "test2016_01.h"
// #include <stdio.h>

#define TEST_VEC_LEN 10

/* module load cudatoolkit/7.5 */
/* use gcc-4.9.3p */
/* nvcc -O2 --expt-extended-lambda -arch compute_35 -std=c++11 main.cu */

int foobar(int i);

int main(int argc, char *argv[])
{
   int *value ;

#if 1
   cudaMallocManaged((void **)&value, sizeof(int)*TEST_VEC_LEN, cudaMemAttachGlobal) ;
#endif
#if 0
// This is what we want to generate in ROSE.
// This code is a problem for ROSE error on "[=]" syntax.
   forall(cuda_traversal(), TEST_VEC_LEN, [=] __device__ (int i) {
       value[i] = i ;
   } ) ;
#else
// This is compilable by ROSE (but not relevant).
   forall(cuda_traversal(), TEST_VEC_LEN, [=] (int i) __attribute__((my_device)) {
       value[i] = i ;
   } ) ;
#endif

   cudaDeviceSynchronize() ;

#if 0
   for (int i=0; i<TEST_VEC_LEN; ++i) {
      printf("%d ", value[i]) ;
   }
   printf("\n") ;
#endif

   return 0 ;
}

