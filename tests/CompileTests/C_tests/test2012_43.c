// Interesting bug in ROSE: compile-time assertion macros expance to something a bit complex.
// could have used: "assert(1 <= I_RING_SIZE);" which would have been simpler...
// This is the last bug in ROSE specific to compiling the grep-2.14 project (100K C application).

// standard C header file
#include <stdbool.h>

// Header file from the grep source code: grep-2.14/lib/verify.h
#include "grep_verify.h"

enum { I_RING_SIZE = 4 };

// This macro (below) expands to: 
//    extern int (*_gl_verify_function21 (void)) [(!!sizeof (struct { unsigned int _gl_verify_error_if_negative: (1 <= I_RING_SIZE) ? 1 : -1; }))];
// I have confirmed that ROSE properly compiles this (it is represented properly in the AST), but unparses it as:
//    extern int (*_gl_verify_function2())[!(!(sizeof(struct )))];
// because who would have expected a class definition in the input argument to the sizeof() function... or at least I didn't...but I will fix this...
// verify (1 <= I_RING_SIZE);

extern int (*_gl_verify_function18 (void)) [(!!sizeof (struct { unsigned int _gl_verify_error_if_negative: (1 <= I_RING_SIZE) ? 1 : -1; }))];
