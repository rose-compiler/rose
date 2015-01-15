#include <stdlib.h>

bool foo() {
  exit (1);
  return false;
}

int main(int, char**) {
  bool x;
  x = false && foo();
  return 0;
}
