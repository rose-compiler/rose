/*******************************************************************
This file provides an example usage of upc_all_reduceL collective
function as described in Section 7.3.2.1 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
#define BLK_SIZE 3
shared [BLK_SIZE] long A[NELEMS*THREADS];
shared long B;

int main()
{
    int i;
    
    upc_forall(i=0;i<NELEMS*THREADS;i++;&A[i])
       A[i] = i;

    upc_barrier;
    upc_all_reduceL(&B, A, UPC_ADD, NELEMS*THREADS, BLK_SIZE,
                     NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    return 0;
}
