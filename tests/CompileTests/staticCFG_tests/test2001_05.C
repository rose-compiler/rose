// This test code is representative of autoconf tests 
// the use of include of a local header file forces the local header file to
// be regenerated (to form a "rose_confdefs.h file) if the header file is
// in the local directory.

// The following test demonstrates the use of a generated header file.

#include "test2001_05.h"

int main()
   {
     return 0;
   }

