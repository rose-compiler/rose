#include "OOtest1b.h"

int main() {
  A* a=new A();
  B* b=new B();
  delete a;
  delete b;
  return 0;
}
