/*******************************************************************
This file provides an example usage of upc_all_permute collective
function as described in Section 7.3.1.6 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS], B[NELEMS*THREADS];
shared int P[THREADS];

int main()
{
    int i;
    
    upc_forall(i=0;i<NELEMS*THREADS;i++;&A[i])
       A[i] = MYTHREAD;
    P[MYTHREAD] = (MYTHREAD+1)%THREADS;

    upc_barrier;
    upc_all_permute( B, A, P, sizeof(int)*NELEMS,
                UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    return 0;
}
