#include "lib.h"

int main(int argc, char** argv) {
#if 0
  int i;
  for (i = 0; i < 5; ++i) {}
#endif
#if 1
  volatile int z;
  // This program takes 4 basic blocks (5 with btorUnroll) to fail by default.
  // Each of these lines adds 1 more block to the run time (number of clock
  // cycles that needs to be tested by the BMC).
  int j;
// DQ (2/11/2009): This is an error for the 4.3.2 compiler (Error: symbol `.l1' is already defined)
// asm volatile (".l1: loop .l1" : "=c" (j) : "0" (4) : "cc");
// asm volatile ("loop l1" : "=c" (j) : "0" (4) : "cc");

  // DQ (2/11/2009): Uncommented this loop instead...
  // for (z = 0; z < 10; ++z) {}
     for (z = 0; z < 10; ++z) {}

#if 0
  for (j = 0; j < 2; ++j) {
    if (z == 0) {++z;}
  }
#endif
#endif
  if (argc < 2) exit(1);
  print(argv[1]); print("\n");
  exit(0);
}
