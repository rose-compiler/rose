#include "lib.h"

int main(int argc, char** argv) {
#if 0
  int i;
  for (i = 0; i < 5; ++i) {}
#endif
  if (argc < 2) exit(1);
  print(argv[1]); print("\n");
  exit(0);
}
