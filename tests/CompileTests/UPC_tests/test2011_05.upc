#include <upc.h>
#include <upc_collective.h>
#include <upc_io.h>

#define NELEMS 10

shared [NELEMS] int scatter_B[NELEMS*THREADS];

int main()
{
   upc_off_t nbytes;
  
   /* Example of upc_blocksizeof() operator */
   nbytes = upc_blocksizeof(scatter_B);
   nbytes = upc_blocksizeof(int);

   /* Example of upc_localsizeof() operator */
   nbytes = upc_localsizeof(scatter_B);
   nbytes = upc_localsizeof(int);

   /* Example of upc_elemsizeof() operator */
   nbytes = upc_elemsizeof(scatter_B);
   nbytes = upc_elemsizeof(int);

   return 0;
}

