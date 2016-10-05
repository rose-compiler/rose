/* DQ (2/3/2011): Bug report from iastate: arg_with_THREADS_reparse/short_test.upc */

#include <upc.h>
#include <upc_collective.h>
#include <upc_io.h>

#define NELEMS 10

shared int *scatter_A;

int main ()
   {
  // Before turning off the EDG constant folding for UPC THREADS and MY_THREAD pseudo-constants.
  //    scatter_A = ((shared[1] int *)(upc_all_alloc((THREADS ),(THREADS ))));
  // After turning off the EDG constant folding for UPC THREADS and MY_THREAD pseudo-constants.
  //    scatter_A = ((shared[1] int *)(upc_all_alloc((THREADS ),(((((THREADS ) * 10)) * (sizeof(int )))))));

  // New behavior after EDG bug fix.
  // Without the -rose:upc_threads 1 option this is unparsed as:
  //    scatter_A = ((shared[1] int *)(upc_all_alloc((THREADS ),(((((THREADS ) * 10)) * (sizeof(int )))))));
  // With the -rose:upc_threads 1 option this is unparsed as:
  //    scatter_A = ((shared[1] int *)(upc_all_alloc((1),((((1 * 10)) * (sizeof(int )))))));

  // Original code
     scatter_A = (shared int *)upc_all_alloc(THREADS, THREADS*NELEMS*sizeof(int));

     return 0;
   }

