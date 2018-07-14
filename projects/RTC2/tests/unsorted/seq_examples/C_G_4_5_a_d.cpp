#include "RuntimeSystem_ParsingWorkaround.h"
/*
********************************************************************************
     Copyright (c) 2005 Iowa State University, Glenn Luecke, James Coyle, James Hoekstra, Marina Kraeva, Olga Taborskaia, Andre Wehe, Ying Xu, and Ziyu Zhang, All rights reserved.  Licensed under the Educational Community License version 1.0.  See the full agreement at http://rted.public.iastate.edu.
  
     Name of the test:  C_G_4_5_a_d.cpp

     Summary:           use "delete" with a pointer and a reference to the value of the pointer

     Test description:  the pointer is a default member variable; the allocation is in the constructor; one deallocation is in the destructor; the reference declaration and the additional deallocation by using the reference are in a member function; "double" is used for the base datatype of the pointer

     Error line:        55

     Support files:     Not needed

     Env. requirements: Not needed

     Keywords:          constructor, member function, deallocate twice, default member variable, delete, destructor, pointer, reference, value

     Last modified:     07/08/2005 15:27

     Programmer:        Andre Wehe, Iowa State University

********************************************************************************
*/

#include <iostream>
#include <cstdlib>

#ifndef EXITCODE_OK
#define EXITCODE_OK 1
#endif

class TestClass {
        // this is the declaration of the pointer
        double *ptrA;

    public:
        TestClass();
        ~TestClass();
        void runA();
};

TestClass :: TestClass() {
    // memory allocation
    std::cout << "memory allocation for ptrA" << std::endl;
    ptrA=new double;

    // initialize
    *ptrA=rand();
}

TestClass :: ~TestClass() {
    // memory deallocation
    std::cout << "memory deallocation of ptrA" << std::endl;
    delete ptrA;
}

void TestClass :: runA() {
    // reference to the value of the pointer
    double &refA=*ptrA;

    // outsmart dead code elimination
    std::cout << "ptrA " << &ptrA << ", " << ptrA << ", " << *ptrA << std::endl;

    // outsmart dead code elimination
    std::cout << "refA " << &refA << ", " << refA << std::endl;

    // memory deallocation
    std::cout << "memory deallocation by using refA" << std::endl;
    delete &refA;
}

int main() {
    TestClass testclassA;
    testclassA.runA();
    return EXITCODE_OK;
}
