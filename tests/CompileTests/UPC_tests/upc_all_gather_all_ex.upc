/*******************************************************************
This file provides an example usage of upc_all_gather_all collective
function as described in Section 7.3.1.4 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS];
shared [NELEMS*THREADS] int B[THREADS][NELEMS*THREADS];

int main()
{
    int i;
    
    upc_forall(i=0;i<NELEMS*THREADS;i++; &A[i])
       A[i] = MYTHREAD;
   
    upc_barrier;
    upc_all_gather_all( B, A, sizeof(int)*NELEMS,
                UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    return 0;
}
