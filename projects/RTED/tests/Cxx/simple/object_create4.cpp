#include "object_create4.h"
#define NULL 0

Test3::Test3() {}

int main() {
  Test t;  
  t.i = 24601;
  t.t3 = NULL;
  int* j;
  j = new int;
  // error, failed to delete j or assign another pointer to its address
  // before it went out of scope

  Test3 test3;
  test3.x=5;

}
