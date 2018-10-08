// This is one of the failing LLVM tests for C.

#include <stdio.h>
int main() {
  static const void *L[] = {&&L1, &&L2, &&L3, &&L4, 0 };
  unsigned i = 0;
  printf("A\n");
L1:
  printf("B\n");
L2:
  printf("C\n");
L3:
  printf("D\n");
  goto *L[i++];
L4: 
  printf("E\n");
  return 0;
}

