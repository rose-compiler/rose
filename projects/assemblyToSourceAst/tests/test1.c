#include "lib.h"

int main(int argc, char** argv) {
  if (argc < 2) exit(1);
  print(argv[1]); print("\n");
  exit(0);
}
