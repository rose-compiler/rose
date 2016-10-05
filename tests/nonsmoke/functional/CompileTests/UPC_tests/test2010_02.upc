/*******************************************************************
This file provides an example usage of upc_all_gather_all collective
function as described in Section 7.3.1.4 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared [NELEMS] int A[NELEMS*THREADS];
shared int *Bdata;

int main()
{
    int i;

 // DQ (9/19/2010): EDG reports a value of type "shared[1] void *" cannot be assigned to an entity of type "shared[1] int *"
 // This is a UPC++ specific issue because we are using UPC with C++ and the C++ has stricter typing than C.
 // Bdata = upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    Bdata = (shared int*) upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    
    upc_forall(i=0;i<NELEMS*THREADS;i++; &A[i])
       A[i] = MYTHREAD;
   
    upc_barrier;
    upc_all_gather_all( Bdata, A, sizeof(int)*NELEMS,
                UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    if(MYTHREAD == 0)
       upc_free(Bdata);

    return 0;
}
