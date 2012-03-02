#include "OOtest2.h"

A::A() {
}

A::~A() {
}

// explicit call of constructor A
B::B():A() {
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
