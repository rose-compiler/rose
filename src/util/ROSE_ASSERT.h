#ifndef ROSE_ASSERT_H
#define ROSE_ASSERT_H

#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROSE_ASSERT
//
// Purpose: Used by ROSE developers to check for their own logic errors when ROSE is configured for debugging. When ROSE is
// configured for production, this macro does nothing (not even evaulate its argument).
//
// Do not use ROSE_ASSERT to simply terminate a process -- ROSE_ASSERT is meant to check for logic errors instead. You could
// use abort, ROSE_ABORT, exit, kill, throw, or other machanisms to unconditionally terminate a process.
//
// Caveats:
//   (1) ROSE_ASSERT is a macro. No overloading, no passing around a pointer, no setting breakpoints, no encapsulation, etc.
//
//   (2) ROSE_ASSERT takes one Boolean argument that should have no side effects.
//
//   (3) ROSE_ASSERT will do nothing (not even evaluate its argument) when NDEBUG is defined.
//
// These semantics were reached by consensus at ROSE meeting 2021-03-23. If you make behavioral changes here, please discuss
// them first.
//
// We recommend that you use the ASSERT_* macros instead, which hook into ROSE's user configurable diagnostics framework. You
// can find documentation here:
//    + ASSERT_* macros: http://rosecompiler.org/ROSE_HTML_Reference/namespaceSawyer_1_1Assert.html
//    + Rose::Diagnostics: http://rosecompiler.org/ROSE_HTML_Reference/namespacerose_1_1Diagnostics.html
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROSE_ASSERT
    #if _MSC_VER
        #define ROSE_ASSERT assert
    #elif defined(__APPLE__) && defined(__MACH__)
        // Pei-Hung (06/16/2015) Sawyer is turned off for Mac OSX
        #define ROSE_ASSERT assert
    #elif defined(__sun)
        // PP (05/16/19)
        #define ROSE_ASSERT assert
    #elif defined(ROSE_ASSERTION_BEHAVIOR)
        #ifdef __GNUC__
            // Pei-Hung (6/16/2015): Using Sawyer ASSERT will consume more than 4GB memory when building ROSE with 32-bit GCC
            // in version 4.2.4 If building ROSE with GCC older than version 4.4, turn off support for Sawyer assert.
            #include <features.h>
            #if __GNUC_PREREQ(4,4)
                #define ROSE_ASSERT ASSERT_require
            #else
                #define ROSE_ASSERT assert
            #endif
        #else
            #define ROSE_ASSERT ASSERT_require
        #endif
    #else
        #define ROSE_ASSERT assert
    #endif
#endif

#endif

