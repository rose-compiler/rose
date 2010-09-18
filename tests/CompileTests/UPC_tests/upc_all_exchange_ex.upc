/*******************************************************************
This file provides an example usage of upc_all_exchange collective
function as described in Section 7.3.1.5 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS*THREADS] int A[THREADS][NELEMS*THREADS];
shared [NELEMS*THREADS] int B[THREADS][NELEMS*THREADS];

int main()
{
    int i;
    
    for(i=0;i<NELEMS*THREADS;i++)
       A[MYTHREAD][i] = MYTHREAD;

    upc_barrier;
    upc_all_exchange( B, A, sizeof(int)*NELEMS,
                UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    return 0;
}
