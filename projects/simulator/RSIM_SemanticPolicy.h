#ifndef ROSE_RSIM_SemanticPolicy_H
#define ROSE_RSIM_SemanticPolicy_H

/** RSIM Instruction Semantics.
 *
 * Instruction semantics are divided into two layers: an inner layer and an outer layer.  The inner layer operates over the
 * concrete domain and is based on ROSE's PartialSymbolicSemantics (only the concrete, constant-folding features are used); the
 * outer layer is based on ROSE MultiSemantics whose first child is the inner semantics.  The MultiSemantics supports more than
 * one child, and the other children can be easily redefined at compile time.  The simulator doesn't use
 * PartialSymbolicSemantics or MultiSemantics directly, but rather derives new classes whose names begin with "Inner" or
 * "Outer", respectively.
 *
 * The MultiSemantics has a non-zero cost because it calls each child whenever a RISC operation needs to be performed. By
 * default, MultiSemantics has four children consisting of the simulator's inner semantics and three copies of ROSE
 * NullSemantics.
 *
 * The simulator source code can be compiled to either use the inner semantics directly, or to use the inner semantics via
 * outer semantics (in which case the other children of outer semantics are also called).  The determination is made by
 * defining the C preprocessor RSIM_SEMANTICS_VTYPE and RSIM_SEMANTICS_POLICY symbols. */

#include "AsmUnparser_compat.h"         /* needed for unparseInstructionWithAddress() */
#include "PartialSymbolicSemantics.h"


/* ValueType and Policy used throughout the simulator.
 *
 * These can be defined such that the simulator uses the inner semantics directly, or the outer semantics.  Inner semantics
 * might be faster, but outer semantics will allow users to plug in additional semantics that execute at the same time as the
 * inner semantics on a RISC operation-by-operation basis.  The entire simulator library needs to be recompiled when this is
 * changed. */
#define RSIM_SEMANTICS_VTYPE  RSIM_Semantics::InnerValueType
#define RSIM_SEMANTICS_STATE  RSIM_Semantics::InnerState<>
#define RSIM_SEMANTICS_POLICY RSIM_Semantics::InnerPolicy<>


/* Classes from which inner policy classes derive.  The simulator is written so that it should be fairly easy to replace
 * PartialSymbolicSemantics with some other class having a similar interface.  The class must operate in the concrete
 * domain.  We have not tested any semantic layer besides PartialSymbolicSemantics [2012-06-12]. */
#define RSIM_SEMANTICS_INNER_BASE BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics
    


namespace RSIM_Semantics {

    template<size_t nBits>
    class InnerValueType: public RSIM_SEMANTICS_INNER_BASE::ValueType<nBits> {
    public:
        InnerValueType(): RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>() {}
        InnerValueType(uint64_t n): RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(n) {}
        InnerValueType(uint64_t name, uint64_t offset, bool negate=false)
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(name, offset, negate) {}
        template <size_t Len>
        InnerValueType(const InnerValueType<Len> &other)
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(other) {}
    };

    template <template <size_t> class ValueType=InnerValueType>
    class InnerState: public RSIM_SEMANTICS_INNER_BASE::State<ValueType> {};

    /* We use the RSIM_SEMANTICS_INNER_BASE policy. That policy is able to handle a certain level of symbolic computation, but
     * we use it because it also does constant folding, which means that its symbolic aspects are never actually used here. We
     * only have a few methods to specialize this way.  The RSIM_SEMANTICS_INNER_BASE::Memory is not used -- we use a MemoryMap
     * instead since we're only operating on known addresses and values, and thus override all superclass methods dealing with
     * memory. */
    template<
        template <template <size_t> class ValueType> class State = InnerState,
        template <size_t> class ValueType = InnerValueType
        >
    class InnerPolicy: public RSIM_SEMANTICS_INNER_BASE::Policy<State, ValueType> {
    public:
        RSIM_Thread *thread;                        /* Thread to which this policy belongs */
        const RegisterDictionary *regdict;

        RegisterDescriptor reg_eax, reg_ebx, reg_ecx, reg_edx, reg_esi, reg_edi, reg_eip, reg_esp, reg_ebp;
        RegisterDescriptor reg_cs, reg_ds, reg_es, reg_fs, reg_gs, reg_ss;
        RegisterDescriptor reg_eflags, reg_df, reg_tf;

        InnerPolicy(RSIM_Thread *thread)
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
        void push(ValueType<32> n);

        /* Same as the x86_pop instruction */
        ValueType<32> pop();

        /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
        ValueType<1> undefined_() {
            return 1;
        }

        /* Called by X86InstructionSemantics for the HLT instruction */
        void hlt() {
            fprintf(stderr, "hlt\n");
            abort();
        }

        /* Called by RDTSC to return time stamp counter.  The simulator doesn't really have a time stamp counter, so we'll just
         * return the number of instructions simulated (counting the RDTSC itself) instead. */
        ValueType<64> rdtsc() {
            return this->get_ninsns();
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
        void writeRegister(const char *regname, const ValueType<Len> &val) {
            writeRegister<Len>(this->findRegister(regname, Len), val);
        }
        template<size_t Len>
        void writeRegister(const RegisterDescriptor &reg, const ValueType<Len> &val) {
            RSIM_SEMANTICS_INNER_BASE::Policy<State, ValueType>::template writeRegister<Len>(reg, val);
            if (reg.get_major()==x86_regclass_segment) {
                ROSE_ASSERT(0==val.known_value() || 3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
                load_sr_shadow((X86SegmentRegister)reg.get_minor(), val.known_value()>>3);
            }
        }

        /* Reads memory from the memory map rather than the super class. */
        template <size_t Len>
        ValueType<Len> readMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<1> &cond);

        /* Writes memory to the memory map rather than the super class. */
        template <size_t Len>
        void writeMemory(X86SegmentRegister sr, const ValueType<32> &addr,
                         const ValueType<Len> &data,  const ValueType<1> &cond);

    };

    typedef BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<RSIM_SEMANTICS_POLICY, RSIM_SEMANTICS_VTYPE> Dispatcher;
    
    
} // namespace




#endif /* ROSE_RSIM_SemanticPolicy_H */
