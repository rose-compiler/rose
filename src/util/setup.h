#ifndef ROSE_UTILITY_SETUP_H
#define ROSE_UTILITY_SETUP_H

#include "stdio.h"

// include header file for C library of string functions 
#include "assert.h"

#include <exception>

#include "roseInternal.h"

// This is a problem with the SUN CC version 6.0 compiler
#include <sstream>  // This (sstream) should eventually replace calls to strstream.h (Kyle)

// DQ (12/7/2003): g++ 3.x prefers to see <fstream> and use of <fstream> 
//                 or <fstream.h> is setup in config.h so use it here.
#include <fstream>
#include <cassert>
#include <cstdio>
#include <cstdlib> // For abort()

// DQ (1/15/2007): Modern C++ compilers can handle these directly
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <stack>

#ifdef _MSC_VER
// DQ (11/4/2009): MS Visual Studio version of hash_multimap
#include <cliext/hash_map>
#else
// DQ (11/4/2009): Use the GNU depricated stuff (what works in ROSE at the moment)
#include <ext/hash_map>
#endif

// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector


// endif associated with ROSE_UTILITY_SETUP_H
#endif
