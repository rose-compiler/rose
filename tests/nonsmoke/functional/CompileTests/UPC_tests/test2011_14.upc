#if 0
*Bug #8*. When a parameter is queried from a function call, the query returns the type-casted parameter. This is a problem for argument checking.

For example:
Given the function prototype
      shared void *upc_all_alloc(size_t nblocks, size_t nbytes);

and a function call with wrong parameters (say)
      upc_all_alloc(-1, 5);

We want to catch such argument errors. So we query for it and check it. However when we query it, we get the first argument as ((size_t)(-1)).

So you see the error only gets propagated before the test and can't be caught.
 This was not happening before. We can understand that this might be working for other cases. But this is a hindrance for us. Could this be switched off in some way? To illustrate the problem more clearly, I have attached a small translator, sample input and output file in the attached tar, namely query_args_cast.tar.gz. Please find the same.

#endif

#include <upc.h>

#define BLK_SIZE 4

shared [BLK_SIZE] int *ptrA;

int main() {
   int i;

   ptrA =  (shared [BLK_SIZE] int *)upc_all_alloc(-1, BLK_SIZE * sizeof(int));
 
   return 0;
}

