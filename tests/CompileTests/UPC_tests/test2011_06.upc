#include <upc.h>
#include <upc_collective.h>
#include <upc_io.h>

shared int elemsizeof_param;

int main()
{
   upc_off_t nbytes;

   /* Example of upc_elemsizeof() operator */
   nbytes = upc_elemsizeof(elemsizeof_param);
  
   return 0;
}

