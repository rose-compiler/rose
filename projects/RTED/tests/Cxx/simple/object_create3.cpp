#include "object_create3.h"

int main() {
  struct Test3 test3;
  test3.x=5;

  Test t;  
  t.i = 24601;
  int* j;
  j = new int;
  // error, failed to delete j or assign another pointer to its address
  // before it went out of scope
}
