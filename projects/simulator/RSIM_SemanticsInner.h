#ifndef ROSE_RSIM_SemanticsInner_H
#define ROSE_RSIM_SemanticsInner_H

#include "AsmUnparser_compat.h"         /* needed for unparseInstructionWithAddress() */
#include "PartialSymbolicSemantics.h"

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
 * NullSemantics.  Experiments show that using MultiSemantics with three instances of NullSemantics incurs a 12% penalty in run
 * time.
 *
 * The simulator source code can be compiled to either use the inner semantics directly, or to use the inner semantics via
 * outer semantics (in which case the other children of outer semantics are also called).  The determination is made by
 * defining the C preprocessor RSIM_SEMANTICS_VTYPE and RSIM_SEMANTICS_POLICY symbols.  See the documentatin in the
 * RSIM_SemanticsSettings.h file for details. */
namespace RSIM_Semantics {

    /* Classes from which inner semantic classes derive.  The simulator is written so that it should be fairly easy to replace
     * PartialSymbolicSemantics with some other class having a similar interface.  The class must operate in the concrete
     * domain.  We have not tested any semantic layer besides PartialSymbolicSemantics [2012-06-12]. */
#   define RSIM_SEMANTICS_INNER_BASE BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics


    /** Values manipulated by the simulator's concrete semantics. */
    template<size_t nBits>
    class InnerValueType: public RSIM_SEMANTICS_INNER_BASE::ValueType<nBits> {
    public:
        InnerValueType()
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>() {}
        explicit InnerValueType(uint64_t n)
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(n) {}
        InnerValueType(uint64_t name, uint64_t offset, bool negate=false)
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(name, offset, negate) {}
        template <size_t Len>
        InnerValueType(const InnerValueType<Len> &other)
            : RSIM_SEMANTICS_INNER_BASE::ValueType<nBits>(other) {}
    };

    /** Machine state for the simulator's concrete semantics. */
    template <template <size_t> class ValueType = RSIM_Semantics::InnerValueType>
    class InnerState: public RSIM_SEMANTICS_INNER_BASE::State<ValueType> {};

    /** RISC operations for the simulator's concrete semantics.
     *
     *  The simulator's concrete semantics are based on the Policy defined in RSIM_SEMANTICS_INNER_BASE (a C preprocessor
     *  symbol, probably ROSE's PartialSymbolicSemantics). That policy is able to handle a certain level of symbolic
     *  computation, but we use it because it also does constant folding well, which means that its symbolic aspects are never
     *  actually used here. By subclassing, we only have a few methods to specialize.  The Memory class of
     *  RSIM_SEMANTICS_INNER_BASE is not used -- instead, we use the concrete MemoryMap stored in the RSIM_Process, and thus
     *  override all superclass methods dealing with memory access. See ROSE's
     *  BinaryAnalysis::InstructionSemantics::NullSemantics class for documentation for most of the methods herein. */
    template<
        template <template <size_t> class ValueType> class State = RSIM_Semantics::InnerState,
        template <size_t> class ValueType = RSIM_Semantics::InnerValueType
        >
    class InnerPolicy: public RSIM_SEMANTICS_INNER_BASE::Policy<State, ValueType> {
    public:
        RSIM_Thread *thread;                        /* Thread to which this policy belongs */
        const RegisterDictionary *regdict;

        RegisterDescriptor reg_eax, reg_ebx, reg_ecx, reg_edx, reg_esi, reg_edi, reg_eip, reg_esp, reg_ebp;
        RegisterDescriptor reg_cs, reg_ds, reg_es, reg_fs, reg_gs, reg_ss;
        RegisterDescriptor reg_eflags, reg_df, reg_tf;

        InnerPolicy()
            : thread(NULL), regdict(NULL) {
            // must call ctor() before using this object
        }

        InnerPolicy(RSIM_Thread *thread)
            : thread(NULL), regdict(NULL) {
            ctor(thread);
        }

        /* Constructor helper. */
        void ctor(RSIM_Thread *thread);

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

        /* Returns the register dictionary. See ROSE's NullSemantics. */
        const RegisterDictionary *get_register_dictionary() const {
            return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
        }

        /* Sets the register dictionary. See ROSE's NullSemantics. */
        void set_register_dictionary(const RegisterDictionary *regdict) {
            this->regdict = regdict;
        }

        /** Returns the current state.  This is defined so that the Inner and Outer semantic policies both have the same
         *  interface for obtaining the concrete state. */
        State<ValueType>& get_concrete_state() {
            return this->get_state();
        }

        /** Segment shadow registers, one per segment register. */
        SegmentInfo sr_shadow[6];

        /** Loads shadow register with an entry from the GDT. */
        void load_sr_shadow(X86SegmentRegister, unsigned gdt_idx);

        /** Return the message object for a tracing facility.  Delegates to the RSIM_Thread. */
        RTS_Message *tracing(TracingFacility what) const;

        /** Return concrete value of EFLAGS register */
        uint32_t get_eflags();

        /** Print machine register state for debugging */
        void dump_registers(RTS_Message*);

        /** Same as the x86_pop instruction.  This is used during signal handling so the simulator can pop a word from the top
         * of the stack without first generating an x86 POP instruction. */
        ValueType<32> pop();

        /* See ROSE's NullSemantics. Redefined here so that we don't end up with non-concrete values in the simulator
         * state. This is called by X86InstructionSemantics for the x86 AND instruction to set AF flag. */
        template<size_t nBits>
        ValueType<nBits> undefined_() {
            return ValueType<nBits>(1);
        }

        /* Called by X86InstructionSemantics for the HLT instruction. See ROSE's NullSemantics. */
        void hlt() {
            fprintf(stderr, "hlt\n");
            abort();
        }

        /* Called by RDTSC to return time stamp counter.  The simulator doesn't really have a time stamp counter, so we'll just
         * return the number of instructions simulated (counting the RDTSC itself) instead.  See ROSE's NullSemantics. */
        ValueType<64> rdtsc() {
            return ValueType<64>(this->get_ninsns());
        }

        /* Called by X86InstructionSemantics for the CPUID instruction. See ROSE's NullSemantics. */
        void cpuid();

        /* Called by X86InstructionSemantics for the INT instruction. See ROSE's NullSemantics. */
        void interrupt(uint8_t num);

        /* Called by X86InstructionSemantics for the SYSENTER instruction. See ROSE's NullSemantics. */
        void sysenter();

        /* Called by X86InstructionSemantics. See ROSE's NullSemantics. */
        void startInstruction(SgAsmInstruction* insn);

        /* Write value to a segment register and its shadow. See ROSE's NullSemantics. */
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

        /* Reads memory from the memory map rather than the super class. See ROSE's NullSemantics. */
        template <size_t Len>
        ValueType<Len> readMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<1> &cond);

        /* Writes memory to the memory map rather than the super class. See ROSE's NullSemantics. */
        template <size_t Len>
        void writeMemory(X86SegmentRegister sr, const ValueType<32> &addr,
                         const ValueType<Len> &data,  const ValueType<1> &cond);

    };
    
}

#endif
