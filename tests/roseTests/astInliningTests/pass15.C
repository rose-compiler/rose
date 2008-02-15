#include <stdlib.h>

bool foo() {
  exit (1);
  return false;
}

int main(int, char**) {
  bool x;
  x = true || foo();
  return 0;
}
