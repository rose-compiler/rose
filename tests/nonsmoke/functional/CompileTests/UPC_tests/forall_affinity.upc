#include<stdio.h>
#include "upc.h"
shared [2] int arr[10*THREADS];
int main()
{

  int i;
  upc_forall (i=0; i<10; i++;&arr[i]){
    printf("thread %d of %d threads performing %d iteration.\n",\
	MYTHREAD,THREADS,i);
  }

/*there is no implicit barrier after upc_forall*/
upc_barrier;

/* chunksize is 2 now for loop iteration scheduling */
  for (i=0;i<10;i++)
    if (MYTHREAD ==upc_threadof(&arr[i]))
      printf("2. thread %d of %d threads performing %d iteration.\n",\
		MYTHREAD,THREADS,i);
  return 0;
}

