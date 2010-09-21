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
    Adata = upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
    myA = (shared [] int *)&Adata[MYTHREAD];

    Bdata = upc_all_alloc(THREADS*THREADS, NELEMS*sizeof(int));
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

