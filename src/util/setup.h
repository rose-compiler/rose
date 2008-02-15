#ifndef ROSE_UTILITY_SETUP_H
#define ROSE_UTILITY_SETUP_H

// #include "rose.h"

#include "stdio.h"

// This does not work with SUN 5.1 C++ compiler
// #include "stream.h"

// include header file for C library of string functions 
// #include "string.h"
#include "assert.h"

#include <exception>

// DQ (1/15/2007): These are depricated and well eventually be removed!
#define FALSE 0
#define TRUE  1

#define ROSE_ASSERT assert
#define ROSE_ABORT abort

// DQ (1/15/2007): Modern C++ compilers can handle these directly
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

// DQ (1/15/2007): This is no longer required for modern C++ compilers!
#if 0
// using namespace std;
#ifndef NAMESPACE_IS_BROKEN
// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;
#endif

#ifdef BOOL_IS_BROKEN
// If BOOL_IS_BROKEN then we can assume that there is no definition for "true" and "false"
#define false 0
#define true  1
typedef int bool;
#endif
#endif

// DQ (1/15/2007): This is not longer required for modern C++ compilers!
#if 0
/* BP : 11/16/2001 */
#ifndef STL_LIST_IS_BROKEN
#include STL_LIST_HEADER_FILE
#endif

/* BP : 11/16/2001 */
#ifndef STL_VECTOR_IS_BROKEN
#include STL_VECTOR_HEADER_FILE
#endif

/* BP : 11/16/2001 */
#ifndef STL_STRING_IS_BROKEN
#include STL_STRING_HEADER_FILE
// const int & npos = string::npos;  // used to indicate not a position in the string
#else
#include <string.h>      /* include the C header file by the same name */
// include <roseString.h> /* internally we typedef roseString to string so that we have a string class to work with */
#endif

// This does not work with SUN 5.1 C++ compiler, but is required for GNU g++
#ifdef __GNUC__
// DQ (12/7/2003): use platform independent macro defined in config.h
#include STL_ALGO_HEADER_FILE
#endif

#include FSTREAM_HEADER_FILE
#endif

// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector


// endif associated with ROSE_UTILITY_SETUP_H
#endif
