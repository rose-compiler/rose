#ifndef auto_tuning_lib_INCLUDED
#define auto_tuning_lib_INCLUDED

//support both C and C++
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*funcPointerT)(void **argv);

// dlopen support  
#include  <dlfcn.h>

// Open a shared library and find a function
funcPointerT findFunctionUsingDlopen(const char* function_name, const char* lib_name);

//Close the shared library's handle
int closeLibHandle();

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif
