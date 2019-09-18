// This test code demonstrates the use of the asm command to
// mix assembly language with C or C++ code.

// It is also demonstrates a bug since it does not unparse correctly
// even though the result compiles.

// Typical use of asm commnd: asm("assembly instruction here");

// #include <stdio.h>
// #include <stdlib.h>

void
foo()
   {
	  long number = 0;
	  unsigned position = 0;

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
#else
  // This will be unparsed as: asm volatile ("bsr %1, %0" : "=r" (position) : "r" (number));
     asm ("bsr %1, %0" : "=r" (position) : "r" (number));
#endif

#if 0
// Skip testing on 64 bit systems for now!
// #ifndef __LP64__
#if 1
       // ":" in string does not work in ROSE, this is the bug

       // DQ (1/6/2009): Trying alternative instruction name /tmp/ccIrj7o3.s:28: Error: bad register name `%1'
  // Note that this does not appear to work properly when EDG is compiled with RECORD_RAW_ASM_OPERAND_DESCRIPTIONS == TRUE
  // asm("nop" : "=r" (position) : "r" (number));
  // asm("bsrl %1, %0" : : "=r" (position), "r" (number)); // result with RECORD_RAW_ASM_OPERAND_DESCRIPTIONS == TRUE
  // asm("bsrl %1, %0" : "=r" (position) : "r" (number)); 

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ >= 3)
#else
     asm("bsrl %1, %0" : "=r" (position) : "r" (number));
     asm("bsrl %1, %0" : "=r" (position) : "r" (number));
#endif

#else
  #warning "Not tested on 64 bit systems"
#endif
#endif

   }

