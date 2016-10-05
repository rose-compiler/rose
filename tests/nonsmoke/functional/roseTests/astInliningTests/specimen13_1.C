#include <stdlib.h>

int foo() {
  exit (1);
  return 0;
}

int main(int, char**) {
  int w, x = 7;
  w = x == 8 ? foo() : 0;
  return w;
}
