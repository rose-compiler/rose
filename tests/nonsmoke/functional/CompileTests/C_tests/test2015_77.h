
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

struct arch_vcpu
   {
     struct xsave_struct *xsave_area;
   }; // __attribute__((__aligned__((1 << (7)))));

struct vcpu
   {
     struct arch_vcpu arch;
   };

