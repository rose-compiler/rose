// from UPC Manual Example 2.2.5
#include <upc_relaxed.h>
#include <stdio.h>
#include <math.h>

#define N 1000

shared [] int arr[10];
// For dynamic translation, THREADS may not appear in declarations of 
// shared arrays with indefinite block size
//shared [] int arr[THREADS]; // Must use static translation
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
