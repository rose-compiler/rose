/*
 * This file should not really exit.  There is a bug in ROSE that prevents us
 * from parsing some of the rted tests.  Once this bug is resolved, this file
 * should be deleted, and the includes added in the makefile should be changed
 * to simply include RuntimeSystem.h.
 *
 * The aforementioned bug is exhibited by this simple program:
 *
        #include <iostream>
        class A {
            void foo() { std::cout << "hejsan" << std::endl; }
        };
        int main() {
            return 0;
        }
 *
 */

#ifndef RUNTIMESYSTEM_PARSING_WORKAROUND
#define RUNTIMESYSTEM_PARSING_WORKAROUND

#ifdef __cplusplus

// tps : (10/09/2009) This will not work with a C compiler (using UPC)
#include <iostream>

//~ void RuntimeSystem_work_around_parsing_errors() {
    //~ std::cout << "" << std::endl;
//~ }

#endif

#include "RuntimeSystem.h"

//#ifdef __cplusplus
// to cope with tests written in old style C++
// using namespace std;
//#endif

#endif
