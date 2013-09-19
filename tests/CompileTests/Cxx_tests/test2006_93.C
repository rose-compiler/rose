// This example demonstrates the use of "GNU statement expressions"
// a gnu extension to C, which I guess we need to support since this
// is a bug submitted by IBM :-).

//Milind Chabbi (9/17/2013): Changes to name names to avoid conflit with global name space during transformations

typedef long int __fd_mask_rose_test;

typedef struct {
    __fd_mask_rose_test fds_bits[1024 / (8 * sizeof (__fd_mask_rose_test))];
} fd_set_rose_test;

int main(int argc, char **argv) {
   fd_set_rose_test *readfds_;
   (__extension__ ({
      register char __result;

// Skip testing on 64 bit systems for now!
#ifndef __LP64__
// TOO (2/16/11): error for Thrifty g++ 3.4.4
#if __GNUC__ != 3
   // Tests of asm are included in test2006_98.C and test2006_99.C.
      __asm__ __volatile__ ("btl %1,%2 ; setcb %b0" : "=q" (__result) : "r"
         (((int) (10)) % (8 * sizeof (__fd_mask_rose_test))), "m" (((readfds_)->fds_bits)[((10) / (8 * sizeof (__fd_mask_rose_test)))])
         : "cc");
#else
  #warning "Not tested on GCC 3.x"
#endif
#else
  #warning "Not tested on 64 bit systems"
#endif

      __result;
   }));
   return 0;
}


