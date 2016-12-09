
/*
// Example of asm code from test2006_93.C (from Yarden, plus some I have found on the web)

typedef long int __fd_mask;
typedef struct {
    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];
} fd_set;

void foo()
   {
     fd_set *readfds_;

     register char __result;
     __asm__ __volatile__ ("btl %1,%2 ; setcb %b0" : "=q" (__result) : "r"
        (((int) (10)) % (8 * sizeof (__fd_mask))), "m" (((readfds_)->fds_bits)[((10) / (8 * sizeof (__fd_mask)))])
        : "cc");
     __result;
   }


*/

typedef long int __fd_mask;

// Skip testing on 64 bit systems for now!
#ifndef __LP64__

// Skip version 4.x gnu compilers
// TOO (2/15/2011): error for Thrifty g++ 3.4.4
#if ( __GNUC__ == 3 && __GNUC_MINOR__ != 4)

typedef struct {
    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];
} fd_set;

void foo0()
   {
  // As of GCC version 3.1, one may write [name] instead of the operand number for a matching constraint. For example:

  // DQ (7/23/2006): Note that this will be unparsed as: register int *result;
     register int *result asm ("cx");
#if 0
  // EDG does not appear to support this ("[name]" syntax), though it should be allowed in anything compatable with gnu past version 3.1.
     asm ("cmoveq %1,%2,%[result]"
          : [result] "=r"(result)
          : "r" (test), "r"(new), "[result]"(old));
#endif
   }

void foo1()
   {
  // Sometimes you need to make an asm operand be a specific register, but there's no matching constraint letter 
  // for that register by itself. To force the operand into that register, use a local variable for the operand 
  // and specify the register in the variable declaration. See Explicit Reg Vars. Then for the asm operand, use 
  // any register constraint letter that matches the register:

     register int *p1 asm ("ax") = 0;
     register int *p2 asm ("bx") = 0;
     register int *result asm ("cx");
#ifndef __INTEL_COMPILER
  // Intel reports some sort of internal error here.
     asm ("sysint" : "=r" (result) : "0" (p1), "r" (p2));
#endif
   }

void foo2()
   {
  // In the above example, beware that a register that is call-clobbered by the target ABI will be overwritten by 
  // any function call in the assignment, including library calls for arithmetic operators. Assuming it is a 
  // call-clobbered register, this may happen to r0 above by the assignment to p2. If you have to use such a 
  // register, use temporary variables for expressions between the register assignment and use:

     int t1 = 0;
     register int *p1 asm ("ax") = 0;
     register int *p2 asm ("bx") = &t1;
     register int *result asm ("cx");
#ifndef __INTEL_COMPILER
  // Intel reports some sort of internal error here.
     asm ("sysint" : "=r" (result) : "0" (p1), "r" (p2));
#endif
   }

void foo3()
   {
  // Additional example asm declarations (how to use the 68881's fsinx instruction):

  // This is a problem for the Intel compiler!
#ifndef __INTEL_COMPILER
  // Intel complains that the registers "ax" and "bx" cannot be double, GNU does not seem to notice :-).
     register double angle  asm ("ax") = 0.0;
     register double result asm ("bx") = 0.0;
     asm ("fsinx %1,%0" : "=f" (result) : "f" (angle));
#endif
   }

void foo4()
   {
  // Alternative example asm declarations (how to use the 68881's fsinx instruction):
#if 0
  // EDG does not appear to support this ("[name]" syntax), though it should be allowed in anything compatable with gnu past version 3.1.
     asm ("fsinx %[angle],%[output]"
          : [output] "=f" (result)
          : [angle] "f" (angle));
#endif
   }

// Specification of asm in function parameter declaration does not appear to be allowed
// void foo5( register int a asm ("cx") )
void foo5( register int a )
   {
#ifndef __INTEL_COMPILER
// Intel complains that the registers "ax" and "bx" cannot be double, GNU does not seem to notice :-).
#define sin(parameter) \
   ({ register double __value asm ("ax") = 0.0; \
      register double __arg   asm ("bx") = (parameter); \
      asm ("fsinx %1,%0": "=f" (__value) : "f" (__arg));  \
      __value; })

      double x = 0.0;
   // This does not work (reports and error from g++ directly, even without ROSE),
   // double y = sin(x);
   // but this does work:
      sin(x);
#endif
   }

void foo_Yarden()
   {
     fd_set *readfds_;

     register char __result;

     __asm__ __volatile__ ("btl %1,%2 ; setcb %b0" : // assembler instruction template 
                           "=q" (__result) : // operand constraint
                           "r" (((int) (10)) % (8 * sizeof (__fd_mask))), "m" (((readfds_)->fds_bits)[((10) / (8 * sizeof (__fd_mask)))]) : // operand constraint
                           "cc", "memory"); // clobbered register list (non gnu-standard register names are ignored)
     __result;
   }

// Example taken from /home/dquinlan2/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs2/bits/atomicity.h (a short header file)
typedef int _Atomic_word;

#ifndef __INTEL_COMPILER
// Intel complains that the input register "m" cannot have a modifier "+"
static inline _Atomic_word 
__attribute__ ((__unused__))
__exchange_and_add (volatile _Atomic_word *__mem, int __val)
{
  register _Atomic_word __result;
  __asm__ __volatile__ ("lock; xadd{l} {%0,%1|%1,%0}"
			: "=r" (__result), "+m" (*__mem) 
                        : "0" (__val)
                        : "memory");
  return __result;
}
#endif

static inline void
__attribute__ ((__unused__))
__atomic_add (volatile _Atomic_word* __mem, int __val)
{
  __asm__ __volatile__ ("lock; add{l} {%1,%0|%0,%1}"
			: "+m" (*__mem) : "ir" (__val) : "memory");
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

#else
   #warning "Not tested on 64 bit systems"
#endif

