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


 // Note: EDG reports that a value of type "shared[1] void *" cannot be assigned to an entity of type "shared[1] int *"
 // A = upc_all_alloc(THREADS, THREADS*NUMELEMS*sizeof(int));
    A = (shared int*) upc_all_alloc(THREADS, THREADS*NUMELEMS*sizeof(int));

    myA = (shared [] int *) &A[MYTHREAD];

 // DQ (9/18/2010): Fixed syntax error (added ";")
 // upc_forall(i=0; i < NUMELEMS*THREADS; i++)
    upc_forall(i=0; i < NUMELEMS*THREADS; i++;)
       myA[i] = i + NUMELEMS*THREADS*MYTHREAD;   // (for example)

    // scatter the SRC_THREAD’s row of the array
    srcA = (shared [] int *) &A[SRC_THREAD];
    upc_barrier;
    upc_all_scatter( B, srcA, sizeof(int)*NUMELEMS,
                 UPC_IN_NOSYNC | UPC_OUT_NOSYNC);
    upc_barrier;

    return 0;
}
