// This bug demonstrates that ";" must be output, even if going into the 
// code generation for the conditional (where it is supressed).

typedef long int __fd_mask;
typedef struct
  {
    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];
  } fd_set;

void foo()
   {
     fd_set *fds;

  // This is what we should be trying to generate (we mostly only miss the ";" in the expression statement, the other minor normalizations appear OK)
  // if (( ( { register char __result; 
  //           asm volatile ("btl %1,%2 ; setcb %b0" : "=q" (__result)
  //                                                 : "r" (((((int )3)) % ((8) * (sizeof(__fd_mask ))))) ,
  //                                                   "m" (((fds -> fds_bits)[((3) / ((8) * (sizeof(__fd_mask ))))]))); 
  //           __result; }))) { }

// TOO (2/15/2011): error for Thrifty g++ 3.4.4
#if __GNUC__ != 3
// Skip testing on 64 bit systems for now!
#ifndef __LP64__

     if ((__extension__ ({register char __result; __asm__ __volatile__ ("btl %1,%2 ; setcb %b0" : "=q" (__result) : "r" (((int) (3)) % (8 * sizeof (__fd_mask))), "m" (((fds)->fds_bits)[((3) / (8 * sizeof (__fd_mask)))]) : "cc"); __result; }))) { }

#else
  #warning "Not tested on 64 bit systems"
#endif
#else
  #warning "Not tested on GCC 3.x"
#endif
   }
