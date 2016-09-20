/* config.h.  Generated automatically by configure.  */
/* config.hin.  Generated automatically from configure.in by autoheader.  */

/* avoid multiple includes */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#if 0
/* START: Evaluate the macros that drive the compilation */
#if defined(COMPILE_APP)
#error "In P++/include/config.h (TOP): COMPILE_APP is defined"
#else
#error "In P++/include/config.h (TOP): COMPILE_APP is NOT defined"
#endif

#if defined(COMPILE_SERIAL_APP)
#error "In P++/include/config.h (TOP): COMPILE_SERIAL_APP is defined"
#else
#error "In P++/include/config.h (TOP): COMPILE_SERIAL_APP is NOT defined"
#endif

#if defined(COMPILE_PPP)
#error "In P++/include/config.h (TOP): COMPILE_PPP is defined"
#else
#error "In P++/include/config.h (TOP): COMPILE_PPP is NOT defined"
#endif

#if defined(SERIAL_APP)
#error "In P++/include/config.h (TOP): SERIAL_APP is defined"
#else
#error "In P++/include/config.h (TOP): SERIAL_APP is NOT defined"
#endif

#if defined(PPP)
#error "In P++/include/config.h (TOP): PPP is defined"
#else
#error "In P++/include/config.h (TOP): PPP is NOT defined"
#endif

#if COMPILE_APP
#error "In P++/include/config.h (TOP): COMPILE_APP is TRUE"
#else
#error "In P++/include/config.h (TOP): COMPILE_APP is FALSE"
#endif

#if COMPILE_PPP
#error "In P++/include/config.h (TOP): COMPILE_PPP is TRUE"
#else
#error "In P++/include/config.h (TOP): COMPILE_PPP is FALSE"
#endif

/* END: Evaluate the macros that drive the compilation */
#endif

/* Avoid defining this without undefining it first (avoids warning message) */
/* if COMPILE_PPP
   error "COMPILE_PPP is already set to TRUE"
   else
   error "COMPILE_PPP should not be re-defined before it is undefined while compiling P++"
   undef COMPILE_PPP
   endif
 */
/* TOP specified: Required for force generation of if defined(COMPILE_PPP) test above */

/* Specify to enable P++ code interfacing with MPI */
#define PXX_ENABLE_MP_INTERFACE_MPI 1

/* Specify to enable P++ code interfacing with PVM */
/* #undef PXX_ENABLE_MP_INTERFACE_PVM */


/* Controls use of assert macro in internal code (should not be controled by use of INTERNALDEBUG) */
/* #undef NDEBUG */

/* Controls use of internal debugging statements and error checking in A++ */
#define INTERNALDEBUG 1

/* Controls use of Pthreads in P++ */
#define USE_PTHREADS 1

/* Controls use of string options which might not be portable (includes string.h) */
#define USE_STRING_SPECIFIC_CODE 1

/* Controls use of PURIFY specific code in P++ source */
/* #undef USE_PURIFY */

/* Controls use of Brian Miller's lower level parallel indirect addressing support in P++ source */
/* #undef USE_PARALLEL_INDIRECT_ADDRESSING_SUPPORT */

/* #undef MPI_IS_LAM */
#define MPI_IS_MPICH 1

/* Controls use of the PADRE Library (a DOE 2000 project) in P++ */
/* #define USE_PADRE 0 */

/* Controls use of the PADRE Library */
#define PARALLEL_PADRE 1

#define HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 1
/* #undef LACKS_EXPLICIT_TEMPLATE_INSTANTIATION */

/* #undef HAVE_BOOL */
#define LACKS_BOOL 0

/* #undef HAVE_NAMESPACE */
#define LACKS_NAMESPACE 1

/* A++/P++ definition of inline, (turned off for GNU) */
#define INLINE inline

/* this will be defined for standard compliant compilers (KCC) */  
/* #undef STD_COMPLIANT_COMPILER */ 

/* Controls use of the PADRE Library (a DOE 2000 project) in P++ */
/* #define USE_PADRE 0 */

/* Name of package */
#define PACKAGE "P++"

/* Version number of package */
#define VERSION "0.7.7f"

/* Signal to user if P++ is used at comple time. */
#define USE_PPP 1

/* Make the name of the C compiler available at compile time. */
#define APP_C_Compiler "cc"

/* Make the name of the C++ compiler available at compile time. */
#define APP_CXX_Compiler "CC"

/* Define if bool type is not properly supported */
#undef BOOL_IS_BROKEN

/* Trigger complete trip through all P++ header files required for P++ compilation */
#define COMPILE_PPP 0

/* Define if not supporting Parti in PADRE */
/* #undef NO_Parti */

/* Define if not supporting Kelp in PADRE */
#define NO_Kelp 1

/* Define if namespace is not properly supported */
#define NAMESPACE_IS_BROKEN 1

/* The STL string class is broken */
#define STL_STRING_IS_BROKEN 1

/* Header file for STL string class template */
/* #undef STL_STRING_HEADER_FILE */

/* The STL list class is broken */
/* #undef STL_LIST_IS_BROKEN */

/* Header file for STL list class template */
#define STL_LIST_HEADER_FILE <list.h>

/* The STL vector class is broken */
/* #undef STL_VECTOR_IS_BROKEN */

/* Header file for STL vector class template */
#define STL_VECTOR_HEADER_FILE <vector.h>

/* The STL string class is broken */
/* #undef STL_ITERATOR_IS_BROKEN */

/* Header file for STL iterator class template */
#define STL_ITERATOR_HEADER_FILE <iterator.h>

/* The STL algorithms are broken */
/* #undef STL_ALGO_IS_BROKEN */

/* Header file for STL algo class template */
#define STL_ALGO_HEADER_FILE <algo.h>

/* Trigger complete trip through all PADRE header files required for PADRE compilation */
#define COMPILE_PADRE 0

/* Define if not supporting GlobalArrays in PADRE */
#define NO_GlobalArrays 1

/* BOTTOM specified: Required for force generation of if defined(COMPILE_PPP) test above */

/* Error checking for the PXX_ENABLE_MP_INTERFACE_MPI and PXX_ENABLE_MP_INTERFACE_PVM variables */
#if defined(PXX_ENABLE_MP_INTERFACE_MPI) && defined(PXX_ENABLE_MP_INTERFACE_PVM)
#error "Both PXX_ENABLE_MP_INTERFACE_MPI and PXX_ENABLE_MP_INTERFACE_PVM are defined"
#endif
#if !defined(PXX_ENABLE_MP_INTERFACE_MPI) && !defined(PXX_ENABLE_MP_INTERFACE_PVM)
#error "Neither PXX_ENABLE_MP_INTERFACE_MPI nor PXX_ENABLE_MP_INTERFACE_PVM are defined"
#endif

/* Error checking for the use of COMPILE_APP while compiling P++ variables */
#if defined(COMPILE_APP)
#error "COMPILE_APP should not be defined while compiling P++"
#endif

/* Error checking for the HAVE_BOOL and LACKS_BOOL variables */
#if defined(HAVE_BOOL) && defined(LACKS_BOOL)
#error "Both HAVE_BOOL and LACKS_BOOL are defined"
#endif
#if !defined(HAVE_BOOL) && !defined(LACKS_BOOL)
#error "Neither HAVE_BOOL nor LACKS_BOOL are defined"
#endif

/* Error checking for the HAVE_NAMESPACE and LACKS_NAMESPACE variables */
#if defined(HAVE_NAMESPACE) && defined(LACKS_NAMESPACE)
#error "Both HAVE_NAMESPACE and LACKS_NAMESPACE are defined"
#endif
#if !defined(HAVE_NAMESPACE) && !defined(LACKS_NAMESPACE)
#error "Neither HAVE_NAMESPACE nor LACKS_NAMESPACE are defined"
#endif

#ifdef BOOL_IS_BROKEN
#ifndef BOOL_IS_TYPEDEFED
typedef int bool;
#define BOOL_IS_TYPEDEFED 0
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif

/* Avoid multiple includes */
/* APP_CONFIG_H */
#endif


