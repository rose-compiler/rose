/*******************************************************************
This file provides an example usage of upc_all_gather collective
function as described in Section 7.3.1.3 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS];

/* 
Note: EDG reports that a dynamic THREADS dimension requires a definite block size "shared []".
shared [] int B[NELEMS*THREADS];
So this test codes has been modified.  We need some expert advice on this subject.
*/
shared [NELEMS] int B[NELEMS*THREADS];

int main()
{
    int i;
    
    upc_forall(i=0;i<NELEMS*THREADS;i++; &A[i])
       A[i] = MYTHREAD;

    upc_all_gather( B, A, sizeof(int)*NELEMS,
                UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC );

    return 0;
}
