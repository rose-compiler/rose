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
       // Changed "bsrl" to "bsr", this works at least on 64 bit systems.
#ifdef ROSE_USE_NEW_EDG_INTERFACE
       // DQ (1/11/2009): This code only works under the new EDG/Sage interface.
          asm("bsr %1, %0" : "=r" (position) : "r" (number));
#endif

#if 0
// Skip testing on 64 bit systems for now!
// #ifndef __LP64__
       // ":" in string does not work in ROSE, this is the bug

       // Original code: asm("bsrl %1, %0" : "=r" (position) : "r" (number));

       // DQ (1/6/2009): Trying alternative instruction name /tmp/ccIrj7o3.s:28: Error: bad register name `%1'

#ifndef ROSE_USE_NEW_EDG_INTERFACE
#ifndef __LP64__
       // With the older EDG/SageIII interface this fails for 64bit systems (though it appears to generate the same code).
          asm("bsrl %1, %0" : "=r" (position) : "r" (number));
#else
   #warning "Not tested on 64 bit systems (using the older EDG/SageIII Interface)..."
          asm("bsrl %1, %0" : "=r" (position) : "r" (number));
#endif
#else
       // The new interface handles this properly (though it appears to generate the same code).
          asm("bsrl %1, %0" : "=r" (position) : "r" (number));
#endif
#endif
          result = position;
        }

     return 0;
   }

