#include "classTest.h"


TestClass::TestClass() {}
TestClass::~TestClass() {}
void TestClass::runA() {}
int TestClass::ptrD;

int main(int argc, char** argv) {
  class TestClass testclassA;
#if 0
  unsigned long add1 = (size_t )&(((TestClass *)1)->ptrA)-1;
  unsigned long add2 = (size_t )&(((TestClass *)1)->ptrB)-1;
  unsigned long add3 = (size_t )&(((TestClass *)1)->ptrC)-1;
  unsigned long add4 = (size_t )&(((TestClass *)1)->ptrD)-1;
  unsigned long add5 = (size_t )&(((TestClass *)1)->ptrE)-1;
#endif 

#if 0
  cerr << "Class 2 " << endl;
  class TestClass2* test2 = reinterpret_cast<TestClass2*>(&testclassA);;
  unsigned long addr1 = (size_t )&(((ROSE::TestClass2 *)0)->ptrA);
  unsigned long addr2 = (size_t )&(((ROSE::TestClass2 *)0)->ptrB);
  unsigned long addr3 = (size_t )&(((ROSE::TestClass2 *)0)->ptrC);
  unsigned long addr4 = (size_t )&(((TestClass *)1)->ptrD)-1;
  unsigned long addr5 = (size_t )&(((ROSE::TestClass2 *)0)->ptrE);


  cerr << "Padding of Class1:   offset(ptrA *): " << add1 << "  offset(ptrB long double): " << add2   << "  offset(ptrC char): " << add3
      << "  offset(ptrD int): " << add4  << "  offset(ptrE *): " << add5 << endl;
  cerr << "Padding of Class2:   offset(ptrA *): " << addr1 << "  offset(ptrB long double): " << addr2 << "  offset(ptrC char): " << addr3
       << "  offset(ptrD int): " << addr4 << "  offset(ptrE *): " << addr5 << endl;
#endif

  return 0;
}



