#ifndef ROSE_PATHS_H
#define ROSE_PATHS_H

// DQ (4/21/2009): If this is not set then set it here.
// For most of ROSE usage this is set in sage3.h, but initial
// construction or ROSETTA used to generate ROSE requires
// it as well.
#if !defined(_FILE_OFFSET_BITS)
   #define _FILE_OFFSET_BITS 64
#endif

// PP (7/1/19): Solaris in 32bit uses _FILE_OFFSET_BITS 32
#if !defined(__sun)
// DQ (4/21/2009): This must be set before sys/stat.h is included by any other header file.
// Use of _FILE_OFFSET_BITS macro is required on 32-bit systems to control the size of "struct stat"
#if !(defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
   #error "The _FILE_OFFSET_BITS macro should be set before any sys/stat.h is included by any other header file!"
#endif
#endif /* __sun */

#include <string>
#include "rosedll.h"

ROSE_UTIL_API extern const std::string ROSE_GFORTRAN_PATH;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_TOP_SRCDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_TOP_BUILDDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_PREFIX;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_DATADIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_BINDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_INCLUDEDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_INFODIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_LIBDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_LIBEXECDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_LOCALSTATEDIR;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_MANDIR;

ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR;

/* Additional interesting data to provide */
ROSE_UTIL_API extern const std::string ROSE_CONFIGURE_DATE;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_BUILD_OS;
ROSE_UTIL_API extern const std::string ROSE_AUTOMAKE_BUILD_CPU;
ROSE_UTIL_API extern const std::string ROSE_OFP_VERSION_STRING;

/** Numeric form of ROSE version.
 *
 *  This is the numeric form of the ROSE version number. It's formed by taking the ROSE version string from the ROSE_VERSION
 *  file at the top of the source tree (e.g., "0.9.6.399-rc1", replacing all non-numeric characters with '.',
 *  ("0.9.6.399...1"), deleting dots that are repeated ("0.9.6.399.1"), discarding all but the first three numeric values
 *  ("0.9.6"), forming an integer using three digits per version part ("000009006"), and removing leading zeros ("9006").
 *
 *  If the first three numbers of a ROSE version are called "major", "minor", and "patch" then these three values can be
 *  obtained from the ROSE_NUMERIC_VERSION with this code:
 *
 * @code
 *   unsigned long patch_number = ROSE_NUMERIC_VERSION % 1000ul;
 *   unsigned long minor_number = ROSE_NUMERIC_VERSION / 1000ul % 1000ul;
 *   unsigned long major_number = ROSE_NUMERIC_VERSION / 1000000ul;
 * @endcode */
extern const unsigned long ROSE_NUMERIC_VERSION;


// DQ (5/2/2009): This is temporary while we work out the details of the new Graph IR nodes.
// #define USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY 0

//#ifdef ROSE_USE_NEW_GRAPH_NODES
//#ifndef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
//#warning "ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY IS NOT SET"
//#endif
//#endif


#endif /* ROSE_PATHS_H */

