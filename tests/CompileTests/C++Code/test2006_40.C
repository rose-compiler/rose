// This test code demonstrates the use of the asm command to
// mix assembly language with C or C++ code.

// It is also demonstrates a bug since it does not unparse correctly
// even though the result compiles.

// Typical use of asm commnd: asm("assembly instruction here");

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
   {
	  long max = atoi(argv[1]);
	  long number;
	  unsigned position;
	  volatile unsigned result;

     for (number = 1; number <= max; ++number)
        {
       // ":" in string does not work in ROSE, this is the bug
          asm("bsrl %1, %0" : "=r" (position) : "r" (number));
          result = position;
        }

     return 0;
   }

