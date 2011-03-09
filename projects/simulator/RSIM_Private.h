/* Private header file. This is used only during compilation. We cannot include "rose_config.h" in a public header file because
 * it excessively pollutes the global name space. Therefore we include it in this private header file along with anything that
 * depends on it.  Then we include this private header file in each of the *.C files but none of the public *.h files. */
#ifndef ROSE_RSIM_Private_H
#define ROSE_RSIM_Private_H

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

/* Define one CPP symbol to determine whether this simulator can be compiled.  The definition of this one symbol depends on
 * all the header file prerequisites.  */
#if defined(HAVE_ASM_LDT_H) && defined(HAVE_ELF_H) &&                                                                          \
    defined(HAVE_LINUX_TYPES_H) && defined(HAVE_LINUX_DIRENT_H) && defined(HAVE_LINUX_UNISTD_H)
#  define ROSE_ENABLE_SIMULATOR
#else
#  undef ROSE_ENABLE_SIMULATOR
#endif

#include "RSIM_Simulator.h"

#endif
