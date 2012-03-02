#include "OOtest1a.h"

A::A() {
}

A::~A() {
}

B::B() {
}

B::~B() {
}

int main() {
  A* a=new A();
  B* b=new B();
  delete a;

  // here the destructor of A is also called
  delete b;

  return 0;
}
