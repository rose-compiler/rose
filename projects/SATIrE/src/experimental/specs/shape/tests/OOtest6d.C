#include "OOtest6d.h"

void A2::nonvirtual_foo() { 
  return;
}
void B2::nonvirtual_foo() { 
  int x;
  A2* a2=new A2();
  a2->nonvirtual_foo(); 
  return;
}

int main() {
  B2* b1=new B2();
  B2* b2=new B2();
  b2->next=b1;
  b2->next->nonvirtual_foo();

  return 0;
}
