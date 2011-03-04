// TOO (2/16/2011): error for Thrifty g++ 3.4.4
#if __GNUC__ != 3

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
     register short angle  asm ("cx") = 0;
     register short result asm ("bx") = 0;
     // JJW fixed this to use a real x86 instruction
     asm ("leaw (%1),%0" : "=S" (result) : "D" (angle));
#endif
   }

#else
  #warning "Not tested on 64 bit systems"
#endif
#else
  #warning "Not tested on GCC 3.x"
#endif
