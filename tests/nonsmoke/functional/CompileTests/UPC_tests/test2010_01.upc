/*******************************************************************
This file provides an example usage of upc_all_exchange collective
function as described in Section 7.3.1.5 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10
shared int *Adata, *Bdata;
shared [] int *myA, *myB;
int i;

int main()
{
 // DQ (9/19/2010): EDG reports a value of type "shared[1] void *" cannot be assigned to an entity of type "shared[1] int *"
 // This is a UPC++ specific issue because we are using UPC with C++ and the C++ has stricter typing than C.
 // Adata = upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    Adata = (shared int*) upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    myA = (shared [] int *)&Adata[MYTHREAD];

 // DQ (9/19/2010): EDG reports a value of type "shared[1] void *" cannot be assigned to an entity of type "shared[1] int *"
 // This is a UPC++ specific issue because we are using UPC with C++ and the C++ has stricter typing than C.
 // Bdata = upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    Bdata = (shared int*) upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    myB = (shared [] int *)&Bdata[MYTHREAD];

    for(i=0;i<NELEMS*THREADS;i++)
       myA[i] = MYTHREAD;

    upc_barrier;
    upc_all_exchange( Bdata, Adata, sizeof(int)*NELEMS,
                UPC_IN_NOSYNC | UPC_OUT_NOSYNC );
    upc_barrier;

    if(MYTHREAD == 0)
    {
       upc_free(Adata);
       upc_free(Bdata);
    }

    return 0;
}

