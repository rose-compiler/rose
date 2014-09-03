#ifndef ROSE_RSIM_SemanticsOuter_H
#define ROSE_RSIM_SemanticsOuter_H

#include "RSIM_SemanticsInner.h"
#include "MultiSemantics.h"

#define RSIM_SEMANTICS_OUTER_BASE                                                                                              \
    rose::BinaryAnalysis::InstructionSemantics::MultiSemantics<                                                                \
        RSIM_SEMANTICS_OUTER_0_VTYPE, RSIM_SEMANTICS_OUTER_0_STATE, RSIM_SEMANTICS_OUTER_0_POLICY,                             \
        RSIM_SEMANTICS_OUTER_1_VTYPE, RSIM_SEMANTICS_OUTER_1_STATE, RSIM_SEMANTICS_OUTER_1_POLICY,                             \
        RSIM_SEMANTICS_OUTER_2_VTYPE, RSIM_SEMANTICS_OUTER_2_STATE, RSIM_SEMANTICS_OUTER_2_POLICY,                             \
        RSIM_SEMANTICS_OUTER_3_VTYPE, RSIM_SEMANTICS_OUTER_3_STATE, RSIM_SEMANTICS_OUTER_3_POLICY                              \
        >


// documented in RSIM_SemanticsInner.h
namespace RSIM_Semantics {

    template<size_t nBits>
    class OuterValueType: public RSIM_SEMANTICS_OUTER_BASE::ValueType<nBits> {
    public:
        OuterValueType()
            : RSIM_SEMANTICS_OUTER_BASE::ValueType<nBits>() {}
        explicit OuterValueType(uint64_t n)
            : RSIM_SEMANTICS_OUTER_BASE::ValueType<nBits>() {
            set_subvalue(RSIM_SEMANTICS_OUTER_BASE::SP0(), RSIM_SEMANTICS_OUTER_0_VTYPE<nBits>(n));
            set_subvalue(RSIM_SEMANTICS_OUTER_BASE::SP1(), RSIM_SEMANTICS_OUTER_1_VTYPE<nBits>(n));
            set_subvalue(RSIM_SEMANTICS_OUTER_BASE::SP2(), RSIM_SEMANTICS_OUTER_2_VTYPE<nBits>(n));
            set_subvalue(RSIM_SEMANTICS_OUTER_BASE::SP3(), RSIM_SEMANTICS_OUTER_3_VTYPE<nBits>(n));
        }
        OuterValueType(bool valid0, const RSIM_SEMANTICS_OUTER_0_VTYPE<nBits> &v0,
                       bool valid1, const RSIM_SEMANTICS_OUTER_0_VTYPE<nBits> &v1,
                       bool valid2, const RSIM_SEMANTICS_OUTER_0_VTYPE<nBits> &v2,
                       bool valid3, const RSIM_SEMANTICS_OUTER_0_VTYPE<nBits> &v3)
            : RSIM_SEMANTICS_OUTER_BASE::ValueType<nBits>(valid0, v0, valid1, v1, valid2, v2, valid3, v3) {}

        bool is_known() const {
            return this->value0.is_known();
        }

        uint64_t known_value() {
            return this->value0.known_value();
        }
    };


    template <template <size_t> class ValueType = OuterValueType>
    class OuterState: public RSIM_SEMANTICS_OUTER_BASE::State<ValueType> {};


    template<
        template <template <size_t> class ValueType> class State = OuterState,
        template <size_t> class ValueType = OuterValueType
        >
    class OuterPolicy: public RSIM_SEMANTICS_OUTER_BASE::Policy<State, ValueType> {
    public:
        RegisterDescriptor &reg_eax, &reg_ebx, &reg_ecx, &reg_edx, &reg_esi, &reg_edi, &reg_eip, &reg_esp, &reg_ebp;
        RegisterDescriptor &reg_cs, &reg_ds, &reg_es, &reg_fs, &reg_gs, &reg_ss;
        RegisterDescriptor &reg_eflags, &reg_df, &reg_tf;

        OuterPolicy(RSIM_Thread *thread)
            : reg_eax(this->policy0.reg_eax), reg_ebx(this->policy0.reg_ebx), reg_ecx(this->policy0.reg_ecx),
              reg_edx(this->policy0.reg_edx), reg_esi(this->policy0.reg_esi), reg_edi(this->policy0.reg_edi),
              reg_eip(this->policy0.reg_eip), reg_esp(this->policy0.reg_esp), reg_ebp(this->policy0.reg_ebp),
              reg_cs(this->policy0.reg_cs), reg_ds(this->policy0.reg_ds), reg_es(this->policy0.reg_es),
              reg_fs(this->policy0.reg_fs), reg_gs(this->policy0.reg_gs), reg_ss(this->policy0.reg_ss),
              reg_eflags(this->policy0.reg_eflags), reg_df(this->policy0.reg_df), reg_tf(this->policy0.reg_tf) {
            this->policy0.ctor(thread);
        }

        RSIM_SEMANTICS_OUTER_0_STATE<>& get_concrete_state() {
            return this->policy0.get_state();
        }

        /** See InnerPolicy::get_ninsns() */
        size_t get_ninsns() const {
            return this->policy0.get_ninsns();
        }

        /** See InnerPolicy::set_ninsns() */
        void set_ninsns(size_t n) {
            this->policy0.set_ninsns(n);
        }

        /** See InnerPolicy::dump_registers() */
        void dump_registers(RTS_Message *m) {
            this->policy0.dump_registers(m);
        }

        /** See InnerPolicy::load_sr_shadow() */
        void load_sr_shadow(X86SegmentRegister sr, unsigned gdt_idx) {
            this->policy0.load_sr_shadow(sr, gdt_idx);
        }

        /** See InnerPolicy::tracing() */
        RTS_Message *tracing(TracingFacility what) const {
            return this->policy0.tracing(what);
        }

        /** See InnerPolicy::get_eflags() */
        uint32_t get_eflags() {
            return this->policy0.get_eflags();
        }

        /** See InnerPolicy::pop() */
        ValueType<32> pop() {
            ValueType<32> retval;
            retval.set_subvalue(RSIM_SEMANTICS_OUTER_BASE::SP0(), this->policy0.pop());
            return retval;
        }
        
        
    };
    
    
    
} // namespace

#endif
