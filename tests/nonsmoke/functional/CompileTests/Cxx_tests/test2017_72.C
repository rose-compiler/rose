// This is one of the failing LLVM tests for C.

// #include <stdio.h>
int main() 
   {
     static const void *L[] = {&&L1, &&L2};
     unsigned i = 0;
  // printf("A\n");
L1:
  // printf("B\n");
     i++;
L2:
  // printf("C\n");
     i++;
  // goto *L[i++];

     return 0;
   }

