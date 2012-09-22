/*

varibale tid and sum are shared, array variable a[], b[], c[] are shared, but both a and b are only read, c[] is written.
A class member is aa, the operation in class definition mul() should have been omp critical to prevent race condition.  

*/




#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


class A{
 public:
      int x;
      A(int i = 0) {x = i;}; 
      void mul(int y) { x =x*y;}
};



int main (int argc, char *argv[]) 
{
int nthreads, tid;
int a[10];
int sum = 0;
int i = 0;
int b[10],c[10];
int error;
//int j =0;
 A aa(5);
/* Fork a team of threads giving them their own copies of variables */
#pragma omp parallel  private(nthreads) 
  {

  /* Obtain thread number */
  tid = omp_get_thread_num();
  printf("Hello World from thread = %d\n", tid);
  
  /* Only master thread does this */
  if (tid == 0) 
    {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }
  #pragma omp critical
      tid = 1;   
    aa.mul(2);

  #pragma omp for reduction(+:sum)
    for( i = 0; i < 10; i++)
      {
       for(int  j =0; j < 10; j++ )
         sum  = sum + i;
      } 

   #pragma omp for
    for(i = 1; i < 9; i++)
       {
           c[i] = a[i] + b[i];
  //         a[i+1] = a[i-1] + a[i];
           sum = sum + error; 
            
        }

 
  }  /* All threads join master thread and disband */

return 0;


}
