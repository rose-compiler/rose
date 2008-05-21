// typedef int a, *b, &c, * const &d;
#include "mpi.h"

double recip(int a) {
  return 1. / a;
}

int main(int, char**) {
  int x = 3;
  return int(5 * recip(x));
}
