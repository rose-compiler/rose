#include <iostream>

using namespace std;

class TestClassClass
{
    public:
  void runA(){};
};


void (TestClassClass::*testclassAPtr)();

int main()
{
    // this is the object of the class TestClassClass
    TestClassClass testclassA;

    // set the pointer testclassAPtr to null
    testclassAPtr= 0;

    // invalid acces by using a null pointer
    cerr << "testclassAPtr : " << testclassAPtr << endl;
    //    cerr << "testclassA.*testclassAPtr : " << (testclassA.*testclassAPtr) << endl;
    (testclassA.*testclassAPtr)();

    return 0;
}
