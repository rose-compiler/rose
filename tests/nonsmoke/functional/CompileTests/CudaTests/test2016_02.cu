#include "test2016_01.h"
#include <stdio.h>

#define TEST_VEC_LEN 10

/* module load cudatoolkit/7.5 */
/* use gcc-4.9.3p */
/* nvcc -O2 --expt-extended-lambda -arch compute_35 -std=c++11 main.cu */

int foobar(int i);

int main(int argc, char *argv[])
{
   int *value ;

   cudaMallocManaged((void **)&value, sizeof(int)*TEST_VEC_LEN, cudaMemAttachGlobal) ;

// C++11 CUDA code works without the "__device__" keyword in front of the lambda.
// forall(cuda_traversal(), TEST_VEC_LEN, [=] __device__ (int i) { value[i] = i ; } ) ;
// forall(cuda_traversal(), TEST_VEC_LEN, [=] (int i) { value[i] = i ; } ) ;

// forall(cuda_traversal(), TEST_VEC_LEN, [=] __attribute__(my_device) (int i) { value[i] = i ; } ) ;
// forall(cuda_traversal(), TEST_VEC_LEN, [=] (int i) { value[i] = i ; } ) ;
// forall(cuda_traversal(), TEST_VEC_LEN, [=] __attribute__((my_device)) (int i) { value[i] = i ; } ) ;
// forall(cuda_traversal(), TEST_VEC_LEN, __attribute__((my_device)) [=] (int i) { value[i] = i ; } ) ;
   forall(cuda_traversal(), TEST_VEC_LEN, [=] (int i) __attribute__((my_device)) { value[i] = i ; } ) ;

   int x __attribute__((my_device));

   cudaDeviceSynchronize() ;

   for (int i=0; i<TEST_VEC_LEN; ++i) {
      printf("%d ", value[i]) ;
   }
   printf("\n") ;

   return 0 ;
}

