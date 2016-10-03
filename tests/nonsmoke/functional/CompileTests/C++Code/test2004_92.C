/*
// When compiling SWIG generated files the EDG front-end reports C++ errors
// in the processing of the "cstdio" header file (line 119), included by iostream.
// This test code reproduces the bug by processing just the two header files
// Python.h and iostream.  Both compile fine separately, but not together.
// they will compile in the referece order (#include <iostream> placed first).

// Also, if "include <iostream>" is included before the  #include <Python.h>
// and #include <iostream> then the file representing the trio of include
// directives will compile just fine.

The errors introduces are:
"/usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/cstdio", line 119: error:
          the global scope has no "fgetpos"
    using ::fgetpos;
            ^
 
"/usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/cstdio", line 121: error:
          the global scope has no "fopen"
    using ::fopen;
            ^
 
"/usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/cstdio", line 126: error:
          the global scope has no "freopen"
    using ::freopen;
            ^
 
"/usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/cstdio", line 129: error:
          the global scope has no "fsetpos"
    using ::fsetpos;
            ^
 
"/usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/cstdio", line 148: error:
          the global scope has no "tmpfile"
    using ::tmpfile;
            ^
*/

// DQ (/22/2004): This is currently my fix for this problem, but there may be a better one!
// If this is uncommented then this file will compile with EDG
#include <iostream>

// These include files will not compile in this order, they will compile in the reverse order.
// They appear in this order when compiling SWIG files.
#include <Python.h>
#include <iostream>

// It is a BUG in the ROSE unparser that a statement is required to trigger the unparsing of the 
// comments and CPP directives above.
// int x;


