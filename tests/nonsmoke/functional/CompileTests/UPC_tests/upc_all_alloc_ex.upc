/***************************************************************
This file provides an example usage of upc_all_alloc collective
function as described in Section 7.2.2.2 in the UPC Spec v1.2.
***************************************************************/

#include <upc.h>
#include <upc_collective.h>

#define NELEMS 10

shared [NELEMS] int* array;

int main()
{
   array = (shared [NELEMS] int*)upc_all_alloc(THREADS, NELEMS*sizeof(int));

   if(MYTHREAD == 0)
     upc_free(array);
   
   return 0;
}
