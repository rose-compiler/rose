#include "OOtest4.h"

A1::~A1() {}
B1::~B1() {}
A2::A2() {}
A2::~A2() {}
B2::B2() {}
B2::~B2() {}

int A1::virtual_foo() { return 11; }
int B1::virtual_foo() { return 12; }
int A2::nonvirtual_foo() { return 21; }
int B2::nonvirtual_foo() { return 22; }

int main() {
  A1* a1=new A1();
  B1* b1=new B1();
  a1->virtual_foo();
  b1->virtual_foo();
  delete a1;
  delete b1;

  A2* a2=new A2();
  B2* b2=new B2();
  a2->nonvirtual_foo();
  b2->nonvirtual_foo();
  delete a2;
  delete b2;

  AT1<int>* at12=new AT2();
  at12->virtual_foo();
  AT2* at2=new AT2();
  at2->virtual_foo();
  return 0;
}
