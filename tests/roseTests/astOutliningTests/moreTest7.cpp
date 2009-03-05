// this is similar to moreTest6.cpp, but does not use a block
// to contain the single statement being outlined.
int i;

void foo()
   {
#pragma rose_outline
  // Reference to global variable
     i=0;
   }
