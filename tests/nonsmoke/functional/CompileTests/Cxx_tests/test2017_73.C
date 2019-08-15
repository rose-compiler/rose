// This is one of the failing LLVM tests for C.

int main() 
   {
     static const void *L[] = {&&L1, &&L2};
     unsigned i = 0;
L1:
     i++;
L2:
     i++;

     goto *L[i++];

     return 0;
   }

