#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <iostream>
// #include "mpi.h"

#if 1
typedef int a, *b, &c, * const &d;

typedef int (*e)(int, double);

typedef struct AAA {
  int x;
} BBB;

struct CCC {
  int y;
} DDD, EEE;

#if 1
double recip(int a) {
  return 1. / a;
}

enum aaaaa {www, www2};

int main(int, char**) {
  int x = 3;
  return int(int(www) * recip(x));
}
#endif
#endif
