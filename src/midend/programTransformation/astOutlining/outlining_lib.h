#ifndef Outlining_lib_INCLUDED
#define Outlining_lib_INCLUDED

#ifdef VX_WORKS
// int open (const char* name, int flags, int mode);
extern "C" int open (const char* name, int flags, int mode);
#else
// C file support (non-boost)
#include <unistd.h>
#endif

//support both C and C++
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>  
// check if a file exists or not
bool rose_dynamic_library_exists( char* filename);  

// function pointer type name 
typedef void (*funcPointerT)(void **argv);

// dlopen support  
#include  <dlfcn.h>

// Open a shared library and find a function
funcPointerT findFunctionUsingDlopen(const char* function_name, const char* lib_name);

//Close the shared library's handle
int closeLibHandle();


// a variable argument helper function, to make the call site as simple as possible
// int count : = 2+ parameter count 
// second parameter: filename of the shared library
// third parameter: function name of the outlined function in the shared library
// the remaining parameters: pointers to arguments to be passed by reference into the function
void findAndCallFunctionUsingDlopen (int num, ... );

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif
