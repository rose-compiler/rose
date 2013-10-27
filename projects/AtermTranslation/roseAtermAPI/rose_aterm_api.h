// This file implementes the Aterm API for use in ROSE, so that Aterm
// applications can seemlessly use ROSE ASTs as ATerms.   We will see 
// if this can be made to work...The similar support outlined in the 
// paper "Fusing a Transformation Language with an Open Compiler"
// took specific advantage of Java languages mechanisms that appears
// to have made this a much simpler job than it reduces to for the 
// C API that we are using within the DTEC project.

// DQ (9/27/2013): This is required to be defined for the 64bit ATerm support.
#if (__x86_64__ == 1)
// 64-bit machines are required to set this before including the ATerm header files.
   #define SIZEOF_LONG 8
   #define SIZEOF_VOID_P 8
#else
// 32-bit machines need not have the values defined (but it is required for this program).
   #define SIZEOF_LONG 4
   #define SIZEOF_VOID_P 4
#endif

// #include <assert.h>
// #define ROSE_ASSERT(x) assert(x)

#include <aterm1.h>
#include <aterm2.h>






#include "rose_aterm1.h"
#include "rose_aterm2.h"
