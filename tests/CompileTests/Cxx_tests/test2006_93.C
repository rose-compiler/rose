// This example demonstrates the use of "GNU statement expressions"
// a gnu extension to C, which I guess we need to support since this
// is a bug submitted by IBM :-).

typedef long int __fd_mask;

typedef struct {
    __fd_mask fds_bits[1024 / (8 * sizeof (__fd_mask))];
} fd_set;

int main(int argc, char **argv)
   {
     fd_set *readfds_;
     (__extension__ (
        {
          register char __result;

// Skip testing on 64 bit systems for now!
// #ifndef __LP64__
#if 1
       // Tests of asm are included in test2006_98.C and test2006_99.C.
       // __asm__ __volatile__ ("btl %1,%2 ; setcb %b0" : "=q" (__result) : "r" (((int) (10)) % (8 * sizeof (__fd_mask))), "m" (((readfds_)->fds_bits)[((10) / (8 * sizeof (__fd_mask)))]) : "cc");

       // DQ (1/10/2009): Changed "btl" to "bt" for 64bit use.
          __asm__ __volatile__ ("bt %1,%2 ; setcb %b0" : "=q" (__result) : "r" (((int) (10)) % (8 * sizeof (__fd_mask))), "m" (((readfds_)->fds_bits)[((10) / (8 * sizeof (__fd_mask)))]) : "cc");
#else
  #warning "Not tested on 64 bit systems"
#endif

          __result;
        }));
     return 0;
   }


