// To make the error happen there need to be two files:

#if 0
// Needed for error:
#include <algorithm>

#include <vector>

std::vector<unsigned> method1();
#endif

#include "test2018_15.h"

// Error:
// error: conversion from 'std::vector<unsigned int>' to non-scalar type 'std::vector<long unsigned int, std::allocator<long unsigned int> >' requested
// std::vector< std::char_traits< unsigned int > ::int_type  , class std::allocator< std::char_traits< unsigned int > ::int_type  >  > local1 = method1();
std::vector<unsigned> local1 = method1();

// int array[sizeof(int_type)];
