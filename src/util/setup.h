#ifndef ROSE_UTILITY_SETUP_H
#define ROSE_UTILITY_SETUP_H

#include "stdio.h"

// include header file for C library of string functions 
#include "assert.h"

#include <exception>

// DQ (1/15/2007): These are depricated and well eventually be removed!
// #define FALSE 0
// #define TRUE  1

// #define ROSE_ASSERT assert
// #define ROSE_ABORT abort

// DQ (1/15/2007): Modern C++ compilers can handle these directly
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector


// endif associated with ROSE_UTILITY_SETUP_H
#endif
