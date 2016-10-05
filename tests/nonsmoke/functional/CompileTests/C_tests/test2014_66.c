// This example is similar to test2012_17.c

// This example if from sigaction.h:
// This example demonstrates where a macro name is expanded twice (first 
// in the AST and then again in the processing of the unparsed file).

// When the declarations, "sigaction_t x1;" are in a header file then we
// have a more complex problem that requires we output the #undef directives
// in the source file where the use of the variable is located.
#include "test2014_66.h"

void foo()
   {
  // The only solution I can think of is to generate a macro such as: "#undef abc" so 
  // that we can avoid this being expaned twice.  Note: rewrapping macros is still not 
  // ready for use in ROSE yet.
  // #undef abc

// DQ (8/28/2015): Modified this to detect backend use (required for ROSE when compiled using the Intel compiler).
/* ROSE MODIFICATION: undefine the self-referential macro: sa_handler */
// #ifdef USE_ROSE
//   #ifndef __EDG__
//     #undef sa_handler
//     #undef sa_sigaction
//   #endif
// #endif
#if defined(USE_ROSE_BACKEND)
   #undef sa_handler
   #undef sa_sigaction
#endif

     x1.sa_handler = 42;
     x1.sa_sigaction = 0L;
     x2[0].sa_sigaction = 0L;
   }
