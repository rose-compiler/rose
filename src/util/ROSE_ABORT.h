#ifndef ROSE_ABORT_H
#define ROSE_ABORT_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROSE_ABORT
//
//   Purpose: Terminates the process with failure exit status.
//
//   This macro is intended to be used when you want to forcibly terminate the process in a way that's easy for debugging,
//   regardless of whether ROSE is configured for debugging or production.
//
//   When ROSE is configured for debugging, it will print the file and line information before terminating with a possible core
//   dump. When ROSE is configured for production, it will terminate with a possible core dump.
//
//   These semantics were reached by consensus at ROSE meeting 2021-03-23. If you make behavioral changes here, please discuss
//   them first.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROSE_ABORT
    #ifdef NDEBUG
        #include <stdlib.h>
        #define ROSE_ABORT() abort()
    #else
        #include <assert.h>
        #define ROSE_ABORT() assert(false)
    #endif
#endif

#endif
