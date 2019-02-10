#ifndef auto_tuning_lib_INCLUDED
#define auto_tuning_lib_INCLUDED

//support both C and C++
#ifdef __cplusplus
extern "C" {
#endif

//TODO
//at_begin_timing() and at_end_timing()
typedef void (*funcPointerT)(void **argv);

// dlopen support  
#include  <dlfcn.h>

// Open a shared library and find a function
funcPointerT findFunctionUsingDlopen(char* function_name, char* lib_name);

//Close the shared library's handle
int closeLibHandle(void);

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif
