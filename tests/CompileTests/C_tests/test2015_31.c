typedef unsigned char uint8_t;

#if 1
struct hvm_intack {
    uint8_t source;
    uint8_t vector;
};
#endif

enum hvm_intsrc {
    hvm_intsrc_none,
    hvm_intsrc_pic,
    hvm_intsrc_lapic,
    hvm_intsrc_nmi,
    hvm_intsrc_mce,
    hvm_intsrc_vector
};

struct vcpu
{
    int vcpu_id;
    int processor;
};

struct hvm_intack hvm_vcpu_has_pending_irq(struct vcpu *v);

struct hvm_intack hvm_vcpu_has_pending_irq(struct vcpu *v)
   {
     return ((struct hvm_intack) { hvm_intsrc_nmi, 2 });
   }
