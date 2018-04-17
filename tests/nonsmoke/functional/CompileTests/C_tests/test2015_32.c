typedef unsigned char uint8_t;

struct hvm_intack {
    uint8_t source;
};

struct hvm_intack hvm_vcpu_has_pending_irq()
   {
     return ((struct hvm_intack) { 42 });
   }
