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

//#include <cstdlib>


// TH(2009-07-15): moving definitions of ROSE_ASSERT and ROSE_ABORT to util/processSupport.(h|C)
#include "processSupport.h"

#ifdef _MSC_VER
// DQ (11/28/2009): MSVC does not permit use of "false" in macros.
#define ROSE_INTERNAL_DEBUG 0
#else
#define ROSE_INTERNAL_DEBUG false
#endif

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

ROSE_DLL_API extern const char* roseGlobalVariantNameList[];

// ifndef ROSE_INTERNAL_H
#endif







