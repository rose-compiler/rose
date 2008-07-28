// roseInternal.h -- internal header file for the ROSE Optimizing Preprocessor

#ifndef ROSE_INTERNAL_H
#define ROSE_INTERNAL_H

// Removed support for this, since C++ has "true" and "false" values already
#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

// These are supported this way so that they can be redefined as required
#ifndef ROSE_ASSERT
#ifndef NDEBUG
#define ROSE_ASSERT assert
#else // We use assert(false) equivalents so often for "should not get here", but we don't want nontrivial side effects in asserts to be run when assert is disabled
#define ROSE_ASSERT(x) do {(void)(__builtin_constant_p(x) ? (x ? 0 : abort()) : 0);} while (0)
#endif
#endif
#ifndef ROSE_ABORT
#define ROSE_ABORT  abort
#endif

#define ROSE_INTERNAL_DEBUG FALSE

#define NEWSYMTABLESIZE 5

#define ROSE_STRING_LENGTH 128

// We use a modified KCC script to arrange the architecture specific
// options with which we then call the main rose driver program.
// When this is used we can't use rose with the debugger
//    (dbx: File '../bin/rose_script' is not in ELF format)
#define USE_ARCHITECTURE_SPECIFIC_SCRIPT FALSE

// this comes from Sage/EDG--it may not be of any real use
extern "C" char* C_output_file_name;

extern int ROSE_DEBUG;

extern const char* roseGlobalVariantNameList[];

// ifndef ROSE_INTERNAL_H
#endif







