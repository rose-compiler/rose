/**
 * bash-4.2$ g++ --version
 * g++ (GCC) 6.1.0
 * Copyright (C) 2016 Free Software Foundation, Inc.
 * This is free software; see the source for copying conditions.  There is NO
 * warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * bash-4.2$ g++ codes/tests/current/frontend--0.C
 * codes/tests/current/frontend--0.C:1:26: error: redeclaration of C++ built-in type ‘char16_t’ [-fpermissive]
 *    typedef unsigned short char16_t;
 *                           ^~~~~~~~
**/

typedef unsigned short char16_t;
