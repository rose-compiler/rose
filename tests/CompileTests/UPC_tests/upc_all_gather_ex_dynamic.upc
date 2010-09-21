/*******************************************************************
This file provides an example usage of upc_all_gather collective
function as described in Section 7.3.1.3 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS];
shared [] int *B;

int main()
{
    int i;
    
    B = (shared [] int *) upc_all_alloc(1,NELEMS*THREADS*sizeof(int));
    
    upc_forall(i=0;i<NELEMS*THREADS;i++; &A[i])
       A[i] = MYTHREAD;

    upc_barrier;
    upc_all_gather( B, A, sizeof(int)*NELEMS,
                UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC );
    upc_barrier;

    if(MYTHREAD == 0)
      upc_free(B);
   
    return 0;
}
