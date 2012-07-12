#include "OOtest2b.h"

A::A() {
  this->val=1;
}

// explicit call of constructor A
B::B():A() {
  this->val=2;
}

int main() {
  B* b=new B();
  return 0;
}
