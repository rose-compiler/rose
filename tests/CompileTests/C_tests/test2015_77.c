typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;


struct xsave_struct
{
    union {
        char x[512];
        struct {
            uint16_t fcw;
            uint16_t fsw;
            uint8_t ftw;
            uint8_t rsvd1;
            uint16_t fop;
            union {
                uint64_t addr;
                struct {
                    uint32_t offs;
                    uint16_t sel;
                    uint16_t rsvd;
                };
            } fip, fdp;
            uint32_t mxcsr;
            uint32_t mxcsr_mask;
        };
    } fpu_sse;

    struct { char x[256]; } ymm;

    char data[];

} __attribute__ ((packed, aligned (64)));


struct arch_vcpu
{
    struct xsave_struct *xsave_area;
} __attribute__((__aligned__((1 << (7)))));


struct vcpu
{
    struct arch_vcpu arch;
};


void fpu_fxrstor(struct vcpu *v)
   {
     typeof(v->arch.xsave_area->fpu_sse) *fpu_ctxt;
     if ( !(fpu_ctxt->fsw & 0x0080) )
        {
        }
   }
