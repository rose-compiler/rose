/* config.h.  Generated automatically by configure.  */
/* config.hin.  Generated automatically from configure.in by autoheader.  */

/* avoid multiple includes */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* TOP specified: Required for force generation of if defined(COMPILE_APP) test above */

/* Controls use of assert macro in internal code (should not be controled by use of INTERNALDEBUG) */
/* #undef NDEBUG */

/* Controls use of internal debugging statements and error checking in A++ */
#define INTERNALDEBUG 1

/* Controls use of PTHREADS in A++ */
#define USE_PTHREADS 1

/* Controls use of PURIFY specific code in A++ source */
/* #undef USE_PURIFY */

/* Controls use of string options which might not be portable (includes string.h) */
#define USE_STRING_SPECIFIC_CODE 1

#define HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 1
/* #undef LACKS_EXPLICIT_TEMPLATE_INSTANTIATION */

#define HAVE_BOOL 1
/* #undef LACKS_BOOL */

/* #undef HAVE_NAMESPACE */
#define LACKS_NAMESPACE 1

/* A++/P++ definition of inline, (turned off for GNU) */
#define INLINE inline

/* this will be defined for standard compliant compilers (KCC) */  
/* #undef STD_COMPLIANT_COMPILER */

/* Set this if C++ does not already support type bool */
/* #undef BOOL_IS_BROKEN */

/* Name of package */
#define PACKAGE "A++"

/* Version number of package */
#define VERSION "0.7.7e"

/* Signal to user if P++ is used at comple time. */
#define USE_PPP 0

/* Make the name of the C compiler available at compile time. */
#define APP_C_Compiler "rose"

/* Make the name of the C++ compiler available at compile time. */
#define APP_CXX_Compiler "rose"

/* Trigger complete trip through all A++ header files required for A++ compilation */
#define COMPILE_APP 1

/* BOTTOM specified: Required for force generation of if defined(COMPILE_APP) test above */

/* Error checking for the use of COMPILE_APP while compiling P++ variables */
/* if !defined(COMPILE_APP)
   error "COMPILE_APP should be defined while compiling A++"
   endif
 */

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

/* Make boolean syntax work even when boolean is broken. */
#ifdef BOOL_IS_BROKEN
#ifndef BOOL_IS_TYPEDEFED
typedef int bool;
#define BOOL_IS_TYPEDEFED 1
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif

/* Avoid multiple includes */
/* endif for APP_CONFIG_H */
#endif



