/*******************************************************************
This file provides an example usage of upc_all_gather collective
function as described in Section 7.3.1.3 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS];

// This statement requires the use of the option "-rose:upc_threads n" 
// where "n" is an integer value to compile with ROSE.
shared [] int B[NELEMS*THREADS];

int main()
{
    int i;
    
    upc_forall(i=0;i<NELEMS*THREADS;i++; &A[i])
       A[i] = MYTHREAD;

    upc_all_gather( B, A, sizeof(int)*NELEMS,
                UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC );

    return 0;
}
