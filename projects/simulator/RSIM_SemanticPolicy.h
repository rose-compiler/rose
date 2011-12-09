#ifndef ROSE_RSIM_SemanticPolicy_H
#define ROSE_RSIM_SemanticPolicy_H

#include "AsmUnparser_compat.h"         /* needed for unparseInstructionWithAddress() */
#include "VirtualMachineSemantics.h"


#define RSIM_SEMANTIC_POLICY    VirtualMachineSemantics::Policy
#define RSIM_SEMANTIC_VTYPE     VirtualMachineSemantics::ValueType



/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that its symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class RSIM_SemanticPolicy: public RSIM_SEMANTIC_POLICY {
public:
    RSIM_Thread *thread;                        /* Thread to which this policy belongs */
    RSIM_SemanticPolicy(RSIM_Thread *thread)
        : thread(thread) {
        ctor();
    }

    void ctor();

    struct SegmentInfo {
        uint32_t base, limit;
        bool present;
        SegmentInfo(): base(0), limit(0), present(false) {}
        SegmentInfo(const user_desc_32 &ud) {
            base = ud.base_addr;
            limit = ud.limit_in_pages ? (ud.limit << 12) | 0xfff : ud.limit;
            present = true;  // present = !ud.seg_not_present && ud.useable; // NOT USED BY LINUX

        }
    };

    /** Segment shadow registers, one per segment register. */
    SegmentInfo sr_shadow[6];

    /** Loads shadow register with an entry from the GDT. */
    void load_sr_shadow(X86SegmentRegister, unsigned gdt_idx);

    /* Delegates to thread. */
    RTS_Message *tracing(TracingFacility what) const;

    uint32_t get_eflags() const;
    void set_eflags(uint32_t);

    /* Print machine register state for debugging */
    void dump_registers(RTS_Message*) const;

    /* Same as the x86_push instruction */
    void push(VirtualMachineSemantics::ValueType<32> n);

    /* Same as the x86_pop instruction */
    VirtualMachineSemantics::ValueType<32> pop();

    /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
    VirtualMachineSemantics::ValueType<1> undefined_() {
        return 1;
    }

    /* Called by X86InstructionSemantics for the HLT instruction */
    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    /* Called by RDTSC to return time stamp counter.  The simulator doesn't really have a time stamp counter, so we'll just
     * return the number of instructions simulated (counting the RDTSC itself) instead. */
    VirtualMachineSemantics::ValueType<64> rdtsc() {
        return get_ninsns();
    }

    /* Called by X86InstructionSemantics for the CPUID instruction */
    void cpuid();

    /* Called by X86InstructionSemantics for the INT instruction */
    void interrupt(uint8_t num);

    /* Called by X86InstructionSemantics for the SYSENTER instruction */
    void sysenter();

    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn);

    /* Write value to a segment register and its shadow. */
    void writeSegreg(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<16> &val);

    /* Reads memory from the memory map rather than the super class. */
    template <size_t Len> VirtualMachineSemantics::ValueType<Len>
    readMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
               const VirtualMachineSemantics::ValueType<1> cond);

    /* Writes memory to the memory map rather than the super class. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
                const VirtualMachineSemantics::ValueType<Len> &data,  VirtualMachineSemantics::ValueType<1> cond);

};


typedef X86InstructionSemantics<RSIM_SemanticPolicy, VirtualMachineSemantics::ValueType> RSIM_Semantics;

#endif /* ROSE_RSIM_SemanticPolicy_H */
