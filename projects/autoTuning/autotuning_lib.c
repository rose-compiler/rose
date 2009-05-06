#include "autotuning_lib.h"
#include <stdio.h>
#include <stdlib.h>

static void * functionLib=0;
static  const char *dlError;
#if 0 //TODO Need a C version of  a binary search tree-based C++ map
struct lookupBufferElement
{
  char* function_name;
   funcPointerT pointer;
};
struct lookupBufferElement lookupBuffer[10000];
#endif
funcPointerT findFunctionUsingDlopen(const char* function_name, const char* lib_name)
{
  funcPointerT result =0;
  
  // overhead is too high for iterative algorithm
  // TODO: buffering this?
  functionLib = dlopen(lib_name, RTLD_LAZY);
  dlError = dlerror();
  if( dlError ) {
    printf("Error: cannot open .so file named: %s\n", lib_name);
    exit(1);
  }
  // find the function
  result = dlsym( functionLib, function_name);
  dlError = dlerror();
  if( dlError )
  {
    printf("Error: cannot find function named:%s within a .so file named %s!\n", function_name, lib_name);
    exit(1);
  }

  return result;
}
#if 1
int closeLibHandle()
{
  int rc;
  rc = dlclose(functionLib);
  dlError = dlerror();
  //printf("Error: in closeLibHandle() dlclose() return-%s-\n",dlError); 
  if( rc ) 
    exit(1);
  return 0;
}
#endif
