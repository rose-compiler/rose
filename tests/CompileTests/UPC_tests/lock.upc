/* from UPC Manual Example 2.2.5
*/
#include "upc_relaxed.h"
#include <stdio.h>
#include <stdlib.h> /*for srand() etc.*/
#include <math.h>

#define N 1000

shared [5] int arr[THREADS]; 
upc_lock_t *lock;
int main (){
  int i=0;
  int index;

  srand(MYTHREAD);
  if ((lock=upc_all_lock_alloc())==NULL)
    upc_global_exit(1);

  upc_forall( i=0; i<N; i++; i){
    index = rand()%THREADS;
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
