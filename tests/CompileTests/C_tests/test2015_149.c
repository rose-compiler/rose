// This is a simplified version of test2015_77.c

#if 1
#include "test2015_149.h"
#else
struct xsave_struct
   {
     union
        {
          struct 
             {
               int fsw;
             };
        } fpu_sse;
   };
#endif

void foobar(struct vcpu *v)
   {
     struct xsave_struct *xsave_area;
     typeof(xsave_area->fpu_sse) * abc;
     abc->fsw = 42;

   }
