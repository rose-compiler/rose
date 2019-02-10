#include "autotuning_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Using hash table did not help too much
#define USE_UTHASH 1

#if USE_UTHASH
#include "uthash.h"

#define MAX_KEY_LEN 256
// A hash struct for function name and its corresponding function pointer
typedef struct ns_struct{
  char func_name[MAX_KEY_LEN]; /* key */
  funcPointerT fp; 
  UT_hash_handle hh; /*required for any hashable data structure using uthash*/
} ns_t;

ns_t* entries = NULL;

#endif

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

#if USE_UTHASH
// internal function to insert a pair to the hash table
static void add_hash_entry(const char* function_name, funcPointerT fpt)
{
  ns_t * s;
  s =malloc (sizeof(ns_t));
  if (!s)
  {
    printf("Fatal error: add_hash_entry() malloc() for s failed!\n");
    assert(0);
  }

#if 0
  if ((s->func_name=malloc(strlen(function_name)+1)) == NULL)
  {
    printf("Fatal error: add_hash_entry() malloc() for s->func_name failed!\n");
    assert(0);
  }
  else
#endif    
  {
    strcpy(s->func_name, function_name);
    s->fp = fpt;
//    HASH_ADD_INT(entries, func_name,s);
    HASH_ADD_STR(entries, func_name,s);
  }
}
// retrieve a value from the hash table based on a key 
static funcPointerT find_hash_entry(char function_name[])
{
  ns_t *s;
//  HASH_FIND_INT(entries, &function_name, s);
  HASH_FIND_STR(entries, function_name, s);
  if (s)
    return s->fp;
  else
    return NULL;
}
#endif

//External interface using a hash table to speedup the process
funcPointerT findFunctionUsingDlopen(char* function_name, char* lib_name)
{
  funcPointerT result =0;
 
#if USE_UTHASH
 result = find_hash_entry(function_name); 
 if (result)
    return result;
 else  
 {
   result = findWithDlopen(function_name,lib_name);
   add_hash_entry(function_name,result);
 }
#else 
  result = findWithDlopen(function_name,lib_name);
#endif 
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
