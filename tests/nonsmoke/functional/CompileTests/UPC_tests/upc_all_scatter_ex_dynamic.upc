/*******************************************************************
This file provides an example usage of upc_all_scatter collective
function as described in Section 7.3.1.2 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NUMELEMS 10
#define SRC_THREAD 1
shared int *A;
shared [] int *myA, *srcA;
shared [NUMELEMS] int B[NUMELEMS*THREADS];

int main()
{
    int i;
    
    // allocate an array distributed across all threads
    A = (shared int *)upc_all_alloc(THREADS, THREADS*NUMELEMS*sizeof(int));
    myA = (shared [] int *) &A[MYTHREAD];

    for(i=0; i<NUMELEMS*THREADS; i++)
       myA[i] = i + NUMELEMS*THREADS*MYTHREAD;   // (for example)
    
    // scatter the SRC_THREAD’s row of the array
    srcA = (shared [] int *) &A[SRC_THREAD];
    upc_barrier;
    upc_all_scatter( B, srcA, sizeof(int)*NUMELEMS,
                 UPC_IN_NOSYNC | UPC_OUT_NOSYNC);
    upc_barrier;

    if(MYTHREAD == 0)
       upc_free(A);

    return 0;
}
