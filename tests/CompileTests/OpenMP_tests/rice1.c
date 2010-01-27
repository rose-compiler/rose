/*
 * Contributed by Yonghong Yan
 *
 *  A tricky case when a variable is used to define array size
 *  Outliner will have trouble to generate a legal array type based on the variable
 *  within the outlined function
 *
 *  int *N = (int *)(((struct OUT__1__1527___data *)__out_argv) -> N_p);
 *   //wrong! N is a pointer type now
 *   // compiler will complain: size of array has non-integer type 
 *  int (*a)[N] = (int (*)[N])(((struct OUT__1__1527___data *)__out_argv) -> a_p); 
 *
 *  A possible fix is to run constant propagation first before OpenMP translation.a
 *  Or adjust N to be *N: the variable substitution for shared variable did not work on the type node, which might be ignored by AST traversal by default.
 *  Or the entire declaration statement is not searched at all!!
 *
 * 1/12/2009
 * */ 
#include <omp.h>
#include <stdio.h>
 
#define N 100000
int main (int argc, char *argv[]) {
   int th_id, nthreads;
   #pragma omp parallel private(th_id)
   {
     th_id = omp_get_thread_num();
     printf("Hello World from thread %d\n", th_id);
     #pragma omp barrier
     if ( th_id == 0 ) {
       nthreads = omp_get_num_threads();
       printf("There are %d threads\n",nthreads);
     }
   }

//   int N = 100000;
   int i, a[N];
 
    #pragma omp parallel for
    for (i = 0; i < N; i++)
        a[i] = 2 * i;
 
   return 0;
 }

