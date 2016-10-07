// This is a problem when compiling using the "-m32" option.
// ROSE does not interprete this options to change the sizes of 
// fundamental types such as: int and size_t.

// This problem is demonstrated in the support for Valgrind.
#if 0
// We need a defintion of "size_t" to compile this file.
#ifdef ROSE_M32BIT
#include<stdint.h>
// Build a 32-bit version of "size_t"
typedef int32_t size_t;
#else
// Build a 64-bit version of "size_t" by reading the system file where it is defined.
#include<stddef.h>
#endif
#endif

#include<stddef.h>

// The use of size_t requs that it be defined above.
typedef size_t UInt;
typedef size_t SizeT;

typedef 
   struct {
      UInt  state:2;    // Reachedness.
      UInt  pending:1;  // Scan pending.  
      union {
         SizeT indirect_szB : (sizeof(SizeT)*8)-3; // If Unreached, how many bytes
                                                   //   are unreachable from here.
         SizeT  clique :  (sizeof(SizeT)*8)-3;      // if IndirectLeak, clique leader
                                                   // to which it belongs.
      } IorC;
   } 
   LC_Extra;


void foo()
   {
     int a = sizeof(size_t);
     struct X
     {
       int a : sizeof(size_t);
     };
     
#if 0
     int x = sizeof(int32_t);
     int y = sizeof(int64_t);
#endif
   }
