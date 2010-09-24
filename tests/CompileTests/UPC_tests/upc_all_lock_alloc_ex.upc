/*******************************************************************
This file provides an example usage of upc_all_lock_alloc collective
function as described in Section 7.2.4.3 in the UPC Spec v1.2.
********************************************************************/

#include <upc.h>
#include <upc_collective.h>

upc_lock_t *lckA;

int main()
{
   lckA = upc_all_lock_alloc();
 
   return 0;
}
