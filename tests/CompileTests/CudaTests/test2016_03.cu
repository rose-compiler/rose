#include "test2016_03.h"
#include <stdio.h>

#define TEST_VEC_LEN 10

/* module load cudatoolkit/7.5 */
/* use gcc-4.9.3p */
/* nvcc -O2 --expt-extended-lambda -arch compute_35 -std=c++11 main.cu */

int main(int argc, char *argv[])
{
   int *value ;

   cudaMallocManaged((void **)&value,
                     sizeof(int)*TEST_VEC_LEN,
                     cudaMemAttachGlobal) ;

// This is what we want users to write.
   forall(cuda_traversal(), TEST_VEC_LEN, [=] (int i) {
       value[i] = i ;
   } ) ;

   cudaDeviceSynchronize() ;

   for (int i=0; i<TEST_VEC_LEN; ++i) {
      printf("%d ", value[i]) ;
   }
   printf("\n") ;

   return 0 ;
}

