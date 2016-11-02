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
// Skip testing on 64 bit systems for now!
#ifndef __LP64__
// TOO (2/15/2011): error for Thrifty g++ 3.4.4
#if __GNUC__ >= 4
       // ":" in string does not work in ROSE, this is the bug
          asm("bsr %1, %0" : "=r" (position) : "r" (number));
#endif
#else
  #warning "Not tested on 64 bit systems"
#endif
          result = position;
        }

     return 0;
   }

