#include "outlining_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

static void * functionLib=0;
static  const char *dlError;

// internal one with direct use with dlopen()
static funcPointerT findWithDlopen(const char* function_name, const char* lib_name)
{
  funcPointerT result =0;
  // overhead is too high for iterative algorithm
  // TODO: buffering this?
  functionLib = dlopen(lib_name, RTLD_LAZY);
  dlError = dlerror();
  if( dlError ) {
    printf("Error: cannot open .so file named: %s with error code:%s\n", lib_name,dlError);
    exit(1);
  }
  // find the function
  result = dlsym( functionLib, function_name);
  dlError = dlerror();
  if( dlError )
  {
    printf("Error: cannot find function named:%s within a .so file named %s, error code:%s .\n", function_name, lib_name,dlError);
    exit(1);
  }
  return result;
}

//External interface using a hash table to speedup the process
funcPointerT findFunctionUsingDlopen(const char* function_name, const char* lib_name)
{
  funcPointerT result =0;
  result = findWithDlopen(function_name,lib_name);
  return result;
}

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

bool rose_dynamic_library_exists( char* filename)
{
  // Function to check for existence of file.
  bool returnValue = false;

#ifdef VX_WORKS

#ifndef F_OK
#define F_OK 42

#endif
  int mode = 42;
  // if ( open( filename.c_str(), F_OK, mode ) != -1 )
  if ( open( filename, F_OK, mode ) != -1 )
#else
    // if ( access( filename.c_str(), F_OK ) != -1 )
    if ( access( filename, F_OK ) != -1 )
#endif  
    { 
      returnValue = true;
    }
    else
    { 
      printf ("Note: lib file=%s is unavailable! \n", filename);
    }

  return returnValue;
}

// a variable argument helper function, to make the call site as simple as possible
// int count : = 2 (fixed lib file name and function name) + parameter count 
//
// second parameter: filename of the shared library
// third parameter: function name of the outlined function in a shared library
// the remaining parameters: pointers to arguments to be passed by reference into the function
void findAndCallFunctionUsingDlopen (int num, ... )
{
  va_list arguments;
  char* lib_name;
  char* func_name;
  int param_count=num-2; // how many parameters we have. it should not exceed 1024
  void** out_argv = (void**) malloc(sizeof(void*)* param_count);

  va_start (arguments, num);

  // Extract the shared lib file name
  lib_name= va_arg(arguments, char*);
  num--;

  // Extract the function name
  func_name= va_arg(arguments, char*);

  // Extract parameters: void* 
  num--;
  int offset=0;
  while (num--)
  {
    out_argv[offset++]= va_arg(arguments, void*);
  }

  va_end ( arguments );

  if (rose_dynamic_library_exists(lib_name)) {
    void (*func_p)(void **);
    func_p= findFunctionUsingDlopen(func_name, lib_name);
    ( *func_p)(out_argv);
  }
}
