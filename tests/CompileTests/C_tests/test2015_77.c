
#include "test2015_77.h"

void fpu_fxrstor(struct vcpu *v)
   {
     typeof(v->arch.xsave_area->fpu_sse) *fpu_ctxt;
     if ( !(fpu_ctxt->fsw & 0x0080) )
        {
        }
   }
