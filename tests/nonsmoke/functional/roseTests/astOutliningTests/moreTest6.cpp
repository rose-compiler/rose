// Global variables are not a problem since they will be passed in
// where they are written, however they could be declared extern 
// where they are read only. Not likely an important optimization.
int i;

void foo()
   {
#pragma rose_outline
        {
       // Reference to global variable
          i=0;
        }
   }
