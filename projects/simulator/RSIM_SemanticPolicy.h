#ifndef ROSE_RSIM_SemanticPolicy_H
#define ROSE_RSIM_SemanticPolicy_H

#include "AsmUnparser_compat.h"         /* needed for unparseInstructionWithAddress() */
#include "RSIM_Common.h"
#include "VirtualMachineSemantics.h"


#define RSIM_SEMANTIC_POLICY    VirtualMachineSemantics::Policy
#define RSIM_SEMANTIC_VTYPE     VirtualMachineSemantics::ValueType



/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that its symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class RSIM_SemanticPolicy: public RSIM_SEMANTIC_POLICY {
public:
    /* Thrown for signals. */
    struct Signal {
        explicit Signal(int signo): signo(signo) {}
        int signo;
    };

public:
    RSIM_Thread *thread;                        /* Thread to which this policy belongs */
    RSIM_SemanticPolicy(RSIM_Thread *thread)
        : thread(thread) {
        ctor();
    }

    void ctor();

    /* Delegates to thread. */
    FILE *tracing(unsigned what) const;

    uint32_t get_eflags() const;

    /* Print machine register state for debugging */
    void dump_registers(FILE *f) const;

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
