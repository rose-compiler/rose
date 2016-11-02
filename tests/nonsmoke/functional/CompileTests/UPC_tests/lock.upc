/* from UPC Manual Example 2.2.5
*/
#include "upc_relaxed.h"
#include <stdio.h>

/* DQ (1/9/2010): This is a problem for the Intel compiler! */
#ifndef __INTEL_COMPILER
#include <stdlib.h> /*for srand() etc.*/
#endif

#include <math.h>

#define N 1000

shared [5] int arr[THREADS]; 
upc_lock_t *lock;
int main (){
  int i=0;
  int index;

/* DQ (1/9/2010): This is a problem for the Intel compiler! */
#ifndef __INTEL_COMPILER
  srand(MYTHREAD);
#endif
  if ((lock=upc_all_lock_alloc())==NULL)
    upc_global_exit(1);

  upc_forall( i=0; i<N; i++; i){
/* DQ (1/9/2010): This is a problem for the Intel compiler! */
#ifndef __INTEL_COMPILER
    index = rand()%THREADS;
#endif
    upc_lock(lock);
    arr[index]+=1;
    upc_unlock(lock);
  }

  upc_barrier;

  if( MYTHREAD==0 ) {
    for(i=0; i<THREADS; i++)
      printf("TH%2d: # of arr is %d\n",i,arr[i]);
    upc_lock_free(lock);
  }

  return 0;
}
