#include <upc.h>
#include <upc_collective.h>
#include <upc_io.h>

#define NELEMS 10

shared [NELEMS] int scatter_B[NELEMS*THREADS];

int main()
{
   upc_off_t nbytes;

   /* Example of upc_localsizeof() operator */
   nbytes = upc_localsizeof(scatter_B);
  
   return 0;
}

