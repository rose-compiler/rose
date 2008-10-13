#include "lib.h"

int main(int argc, char** argv) {
#if 0
  int i;
  for (i = 0; i < 5; ++i) {}
#endif
#if 0
  volatile int z;
  // This program takes 4 basic blocks to fail by default.  Each of these lines
  // adds 1 more block to the run time (number of clock cycles that needs to be
  // tested by the BMC).
  if (z == 0) {++z;}
  if (z == 0) {++z;}
  if (z == 0) {++z;}
  if (z == 0) {++z;}
  if (z == 0) {++z;}
#endif
  if (argc < 2) exit(1);
  print(argv[1]); print("\n");
  exit(0);
}
