typedef unsigned int u32;
typedef char bool_t;

struct cpu_info {
//  struct cpu_user_regs guest_cpu_user_regs;
    unsigned int processor_id;
//  struct vcpu *current_vcpu;
    unsigned long per_cpu_offset;
    unsigned long __pad_for_stack_bottom;
};

// static __inline__ struct cpu_info *get_cpu_info(void);
struct cpu_info *get_cpu_info(void);

struct pending_eoi {
    u32 ready:1;
    u32 irq:23;
    u32 vector:8;
};
static __attribute__((__section__(".bss.percpu" ""))) __typeof__(struct pending_eoi) per_cpu__pending_eoi[(0xdf - 0x20 + 1)];
bool_t cpu_has_pending_apic_eoi(void)
{
    return ((((*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__pending_eoi)); (typeof(&per_cpu__pending_eoi)) (__ptr + (get_cpu_info()->per_cpu_offset)); })))[(0xdf - 0x20 + 1)-1].vector) != 0);
}
