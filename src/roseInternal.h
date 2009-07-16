// roseInternal.h -- internal header file for the ROSE Optimizing Preprocessor

#ifndef ROSE_INTERNAL_H
#define ROSE_INTERNAL_H

// DQ (3/22/2009): Added support for detection of Microsoft Compiler (MSVC) details.
#include "rose_msvc.h"

// Removed support for this, since C++ has "true" and "false" values already
// JJW (8/26/2008): Removing these
// #ifndef TRUE
// #define TRUE true
// #endif
// #ifndef FALSE
// #define FALSE false
// #endif

#include <cstdlib>


// TH(2009-07-15): moving definitions of ROSE_ASSERT and ROSE_ABORT to util/processSupport.(h|C)
#include "processSupport.h"
/*
// These are supported this way so that they can be redefined as required
#ifndef ROSE_ASSERT
#ifndef NDEBUG
#define ROSE_ASSERT assert
#else // We use assert(false) equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when assert is disabled
#define ROSE_ASSERT(x) do {if (__builtin_constant_p(x)) {if (x) {} else (std::abort)();}} while (0)
#endif
#endif
#ifndef ROSE_ABORT
#define ROSE_ABORT  abort
#endif
*/

#define ROSE_INTERNAL_DEBUG false

#define NEWSYMTABLESIZE 5

#define ROSE_STRING_LENGTH 128

// We use a modified KCC script to arrange the architecture specific
// options with which we then call the main rose driver program.
// When this is used we can't use rose with the debugger
//    (dbx: File '../bin/rose_script' is not in ELF format)
#define USE_ARCHITECTURE_SPECIFIC_SCRIPT false

// this comes from Sage/EDG--it may not be of any real use
extern "C" char* C_output_file_name;

extern int ROSE_DEBUG;

extern const char* roseGlobalVariantNameList[];

// ifndef ROSE_INTERNAL_H
#endif







