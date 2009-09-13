
// Skip testing on 64 bit systems for now!
#ifndef __LP64__

// DQ (12/4/2006): Note that the 64bit processors appear to use different register names, so this code is not portable.

void foobar()
   {
     int x = 0;
  // register int *asmPtr asm ("ax") = 0;
     register int *asmPtr asm ("bx") = 0;
   }

float x = 0x1.fp3; // Example of hexadecimal floating point representation

float largeConstant = 3.141234567890123456789012345;

void foo()
   {
     register int *p1 asm ("ax") = 0;
#ifndef __INTEL_COMPILER
     register float f1 asm ("bx") = 0x1.fp3;
#endif
     register int int1 asm ("cx"),int2 asm ("dx");
     register int int3 asm ("cx"),int4, int5 asm ("dx");

     int1 = 0;
     int2 = 1;
   }

int main()
   {
     float largeConstant = 3.141234567890123456789012345;

  // Additional example asm declarations (how to use the 68881's fsinx instruction):
#ifndef __INTEL_COMPILER
     register double angle  asm ("cx") = 0;
     register double result asm ("bx") = 0;

   // DQ (1/6/2009): Trying alternative instruction name (similar to test2006_40.C)
   //asm ("fsinx %1,%0" : "=f" (result) : "f" (angle));
     asm ("nop" : "=f" (result) : "f" (angle));
#endif
   }

#else
  #warning "Not tested on 64 bit systems"
#endif

