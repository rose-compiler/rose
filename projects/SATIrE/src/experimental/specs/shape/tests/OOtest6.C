#include "OOtest6.h"

int A2::nonvirtual_foo() { return 21; }
int B2::nonvirtual_foo() { return 22; }

int main() {
  A2* a2=new A2();
  B2* b2=new B2();

  a2->nonvirtual_foo();
  b2->nonvirtual_foo();

  return 0;
}
