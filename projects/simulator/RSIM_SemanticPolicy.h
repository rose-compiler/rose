#ifndef ROSE_RSIM_SemanticPolicy_H
#define ROSE_RSIM_SemanticPolicy_H

#include "AsmUnparser_compat.h"         /* needed for unparseInstructionWithAddress() */
#include "VirtualMachineSemantics.h"


#define RSIM_SEMANTIC_POLICY    BinaryAnalysis::InstructionSemantics::VirtualMachineSemantics::Policy<>
#define RSIM_SEMANTIC_VTYPE     BinaryAnalysis::InstructionSemantics::VirtualMachineSemantics::ValueType



/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that its symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class RSIM_SemanticPolicy: public RSIM_SEMANTIC_POLICY {
public:
    RSIM_Thread *thread;                        /* Thread to which this policy belongs */
    const RegisterDictionary *regdict;

    RegisterDescriptor reg_eax, reg_ebx, reg_ecx, reg_edx, reg_esi, reg_edi, reg_eip, reg_esp, reg_ebp;
    RegisterDescriptor reg_cs, reg_ds, reg_es, reg_fs, reg_gs, reg_ss;
    RegisterDescriptor reg_eflags, reg_df, reg_tf;

    RSIM_SemanticPolicy(RSIM_Thread *thread)
        : thread(thread), regdict(NULL) {
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

    /** Returns the register dictionary. */
    const RegisterDictionary *get_register_dictionary() const {
        return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
    }

    /** Sets the register dictionary. */
    void set_register_dictionary(const RegisterDictionary *regdict) {
        this->regdict = regdict;
    }

    /** Segment shadow registers, one per segment register. */
    SegmentInfo sr_shadow[6];

    /** Loads shadow register with an entry from the GDT. */
    void load_sr_shadow(X86SegmentRegister, unsigned gdt_idx);

    /* Delegates to thread. */
    RTS_Message *tracing(TracingFacility what) const;

    uint32_t get_eflags();
    void set_eflags(uint32_t);

    /* Print machine register state for debugging */
    void dump_registers(RTS_Message*);

    /* Same as the x86_push instruction */
    void push(RSIM_SEMANTIC_VTYPE<32> n);

    /* Same as the x86_pop instruction */
    RSIM_SEMANTIC_VTYPE<32> pop();

    /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
    RSIM_SEMANTIC_VTYPE<1> undefined_() {
        return 1;
    }

    /* Called by X86InstructionSemantics for the HLT instruction */
    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    /* Called by RDTSC to return time stamp counter.  The simulator doesn't really have a time stamp counter, so we'll just
     * return the number of instructions simulated (counting the RDTSC itself) instead. */
    RSIM_SEMANTIC_VTYPE<64> rdtsc() {
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
    template<size_t Len>
    void writeRegister(const char *regname, const RSIM_SEMANTIC_VTYPE<Len> &val) {
        writeRegister<Len>(findRegister(regname, Len), val);
    }
    template<size_t Len>
    void writeRegister(const RegisterDescriptor &reg, const RSIM_SEMANTIC_VTYPE<Len> &val) {
        RSIM_SEMANTIC_POLICY::writeRegister<Len>(reg, val);
        if (reg.get_major()==x86_regclass_segment) {
            ROSE_ASSERT(0==val.known_value() || 3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
            load_sr_shadow((X86SegmentRegister)reg.get_minor(), val.known_value()>>3);
        }
    }

    /* Reads memory from the memory map rather than the super class. */
    template <size_t Len> RSIM_SEMANTIC_VTYPE<Len>
    readMemory(X86SegmentRegister sr, const RSIM_SEMANTIC_VTYPE<32> &addr,
               const RSIM_SEMANTIC_VTYPE<1> &cond);

    /* Writes memory to the memory map rather than the super class. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister sr, const RSIM_SEMANTIC_VTYPE<32> &addr,
                const RSIM_SEMANTIC_VTYPE<Len> &data,  const RSIM_SEMANTIC_VTYPE<1> &cond);

};




typedef BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<RSIM_SemanticPolicy, RSIM_SEMANTIC_VTYPE> RSIM_Semantics;

#endif /* ROSE_RSIM_SemanticPolicy_H */
