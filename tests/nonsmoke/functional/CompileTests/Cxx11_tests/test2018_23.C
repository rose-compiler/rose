#include "test2018_23.h"

// Error:
// error: conversion from 'std::vector<unsigned int>' to non-scalar type 'std::vector<long unsigned int, std::allocator<long unsigned int> >' requested
// std::vector< std::char_traits< unsigned int > ::int_type  , class std::allocator< std::char_traits< unsigned int > ::int_type  >  > local1 = method1();
std::vector<unsigned> local1 = method1();
