/*******************************************************************
This file provides an example usage of upc_all_broadcast collective
function as described in Section 7.3.1.1 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

shared int A[THREADS];
shared int B[THREADS];

int main()
{
   A[MYTHREAD] = MYTHREAD;
 
   upc_barrier;
   upc_all_broadcast(B, &A[1], sizeof(int), UPC_IN_NOSYNC|UPC_OUT_NOSYNC);
   upc_barrier;

   return 0;
}
