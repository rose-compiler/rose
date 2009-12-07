
#include <iostream>
#include <stdlib.h>

using namespace std;

#ifndef SIZE
#define SIZE 100
#endif

#ifndef EXITCODE_OK
#define EXITCODE_OK 1
#endif
  int *arrayA;
class TestClass {
        // this is the declaration of the array
        int *arrayA;

    public:
        TestClass();
        void runA();
};

TestClass :: TestClass() {
    // allocate the array
    cout << "allocate memory for arrayA" << endl;
    arrayA=new int [SIZE];
}

void TestClass :: runA() {
    // declare a pointer and let it point to the array
    int *ptrA=&arrayA[SIZE-1];
}

int main() {
    TestClass testclassA;
    testclassA.runA();
    return EXITCODE_OK;
}
