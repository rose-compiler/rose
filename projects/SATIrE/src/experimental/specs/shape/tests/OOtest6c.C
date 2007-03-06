#include "OOtest6c.h"

A1::~A1() {}
B1::~B1() { this->val=0;}

int main() {
  B1* b1=new B1();
  delete b1;

  return 0;
}
