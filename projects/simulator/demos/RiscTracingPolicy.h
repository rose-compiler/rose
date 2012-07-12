// An instruction semantics domain that prints the names of RISC operations and does nothing else. It could easily be modified
// to sit on top of some other semantic domain to print its operation names as they occur.
//
// In order to use this semantics as described in "Paradigm C" of RSIM_SemanticsSettings.h you'd have to make the following
// adjustments to RSIM_SemanticsSettings.h:
//
//     #define RSIM_SEMANTICS_PARADIGM RSIM_SEMANTICS_PARADIGM_C
//
//     #define RSIM_SEMANTICS_OUTER_1_VTYPE  BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//     #define RSIM_SEMANTICS_OUTER_1_STATE  BinaryAnalysis::InstructionSemantics::NullSemantics::State
//     #define RSIM_SEMANTICS_OUTER_1_POLICY RiscTracingPolicy
//
//     #include "demos/RiscTracingPolicy.h"             // This very file
//
// Then recompile the simulator from scratch and call it and run it like normal (e.g., using the x86sim executable).
// Here's a diff showing the precise changes necessary (as of 2012-06-13):
// 
//     diff --git a/projects/simulator/RSIM_SemanticsSettings.h b/projects/simulator/RSIM_SemanticsSettings.h
//     index b12dcfc..8873901 100644
//     --- a/projects/simulator/RSIM_SemanticsSettings.h
//     +++ b/projects/simulator/RSIM_SemanticsSettings.h
//     @@ -31,7 +31,7 @@
//      #define RSIM_SEMANTICS_PARADIGM_B       1                       /* Paradigm B: Simulator uses "Inner"(-like) semantics */
//      #define RSIM_SEMANTICS_PARADIGM_C       2                       /* Paradigm C: Simulator uses "Outer"(-like) semantics */
//      
//     -#define RSIM_SEMANTICS_PARADIGM RSIM_SEMANTICS_PARADIGM_A       /* The paradigm we are actually using */
//     +#define RSIM_SEMANTICS_PARADIGM RSIM_SEMANTICS_PARADIGM_C       /* The paradigm we are actually using */
//      
//      
//      /** ValueType, State, and Semantic Policy used throughout the simulator.
//     @@ -80,7 +80,7 @@
//      #define RSIM_SEMANTICS_OUTER_0_POLICY   RSIM_Semantics::InnerPolicy
//      #define RSIM_SEMANTICS_OUTER_1_VTYPE    BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//      #define RSIM_SEMANTICS_OUTER_1_STATE    BinaryAnalysis::InstructionSemantics::NullSemantics::State
//     -#define RSIM_SEMANTICS_OUTER_1_POLICY   BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
//     +#define RSIM_SEMANTICS_OUTER_1_POLICY   RiscTracingPolicy
//      #define RSIM_SEMANTICS_OUTER_2_VTYPE    BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//      #define RSIM_SEMANTICS_OUTER_2_STATE    BinaryAnalysis::InstructionSemantics::NullSemantics::State
//      #define RSIM_SEMANTICS_OUTER_2_POLICY   BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
//     @@ -96,6 +96,7 @@
//       *
//       *  All necessary semantics are included here.  This is where the user-defined semantics headers should be included. */
//     +#include "demos/RiscTracingPolicy.h"
//      #include "RSIM_SemanticsInner.h"
//      #include "RSIM_SemanticsOuter.h"
//


#include "NullSemantics.h"

template<
    template <template <size_t> class ValueType> class State = BinaryAnalysis::InstructionSemantics::NullSemantics::State,
    template <size_t> class ValueType = BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
    >
class RiscTracingPolicy: public BinaryAnalysis::InstructionSemantics::NullSemantics::Policy<State, ValueType> {
public:

    typedef BinaryAnalysis::InstructionSemantics::NullSemantics::Policy<State, ValueType> Super;

    RSIM_Thread *thread;

    RiscTracingPolicy() {
        this->set_register_dictionary(RegisterDictionary::dictionary_i386());
    }
    RiscTracingPolicy(RSIM_Thread *thread)
        : thread(thread) {
        this->set_register_dictionary(RegisterDictionary::dictionary_i386());
    }

    void say(const char *opname) {
        static const uint64_t reporting_interval = 100000;      // report only every Nth operation
        static uint64_t nops = 0;                               // number of operations so far
        if (++nops % reporting_interval == 0) {
            char buf[128];
            snprintf(buf, sizeof buf, "RISC operation #%"PRIu64": %s", nops, opname);
#if 0
            // This is the preferred way to spit out diganostics, but it can't be done right here ("thread" has an incomplete
            // type here).  Instead, place this function definition in the RSIM_Templates.h file where RSIM_Thread is a
            // complete type.
            thread->tracing(TRACE_MISC)->mesg("RISC operation: " + s);
#else
            // The lazy way
            std::cerr <<buf <<"\n";
#endif
        }
    }

    void hlt() {
        say("hlt");
        Super::hlt();
    }

    void cpuid() {
        say("cpuid");
        Super::cpuid();
    }

    ValueType<64> rdtsc() {
        say("rdtsc");
        return Super::rdtsc();
    }

    void interrupt(uint8_t n) {
        say("interrupt");
        Super::interrupt(n);
    }

    void sysenter() {
        say("sysenter");
        Super::sysenter();
    }

    template<size_t nBits>
    ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) {
        say("add");
        return Super::add(a, b);
    }

    template<size_t nBits>
    ValueType<nBits> addWithCarries(const ValueType<nBits> &a, const ValueType<nBits> &b, const ValueType<1> &c,
                                    ValueType<nBits> &carry_out) {
        say("addWithCarries");
        return Super::addWithCarries(a, b, c, carry_out);
    }

    template<size_t nBits>
    ValueType<nBits> and_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
        say("and_");
        return Super::and_(a, b);
    }

    template<size_t nBits>
    ValueType<1> equalToZero(const ValueType<nBits> &a) {
        say("equalToZero");
        return Super::equalToZero(a);
    }

    template<size_t nBits>
    ValueType<nBits> invert(const ValueType<nBits> &a) {
        say("invert");
        return Super::invert(a);
    }

    template<size_t BeginBit, size_t EndBit, size_t nBitsA>
    ValueType<EndBit-BeginBit> extract(const ValueType<nBitsA> &a) {
        say("extract");
        return Super::template extract<BeginBit, EndBit>(a);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA+nBitsB> concat(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("concat");
        return Super::concat(a, b);
    }

    template<size_t nBits>
    ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b) {
        say("ite");
        return Super::ite(cond, a, b);
    }

    template<size_t nBits>
    ValueType<nBits> leastSignificantSetBit(const ValueType<nBits> &a) {
        say("leastSignificantSetBit");
        return Super::leastSignificantSetBit(a);
    }

    template<size_t nBits>
    ValueType<nBits> mostSignificantSetBit(const ValueType<nBits> &a) {
        say("mostSignificantSetBit");
        return Super::mostSignificantSetBit(a);
    }

    template<size_t nBits>
    ValueType<nBits> negate(const ValueType<nBits> &a) {
        say("negate");
        return Super::negate(a);
    }

    template<size_t nBits>
    ValueType<nBits> or_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
        say("or_");
        return Super::or_(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> rotateLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("rotateLeft");
        return Super::rotateLeft(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> rotateRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("rotateRight");
        return Super::rotateRight(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> shiftLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("shiftLeft");
        return Super::shiftLeft(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> shiftRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("shiftRight");
        return Super::shiftRight(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> shiftRightArithmetic(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("shiftRightArithmetic");
        return Super::shiftRightArithmetic(a, b);
    }

    template<size_t From, size_t To>
    ValueType<To> signExtend(const ValueType<From> &a) {
        say("signExtend");
        return Super::template signExtend<From, To>(a);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> signedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("signedDivide");
        return Super::signedDivide(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsB> signedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("signedModulo");
        return Super::signedModulo(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA+nBitsB> signedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("signedMultiply");
        return Super::signedMultiply(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA> unsignedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("unsignedDivide");
        return Super::unsignedDivide(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsB> unsignedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("unsignedModulo");
        return Super::unsignedModulo(a, b);
    }

    template<size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
        say("unsignedMultiply");
        return Super::unsignedMultiply(a, b);
    }

    template<size_t nBits>
    ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
        say("xor_");
        return Super::xor_(a, b);
    }

    /** Reads from a named register. */
    template<size_t nBits>
    ValueType<nBits> readRegister(const char *regname) {
        say("readRegister");
        return Super::template readRegister<nBits>(regname);
    }

    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
        say("readRegister");
        return Super::template readRegister<nBits>(reg);
    }

    template<size_t nBits>
    void writeRegister(const char *regname, const ValueType<nBits> &a) {
        say("writeRegister");
        Super::writeRegister(regname, a);
    }

    template<size_t nBits>
    void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &a) {
        say("writeRegister");
        Super::writeRegister(reg, a);
    }

    template<size_t nBits>
    ValueType<nBits> readMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<1> &cond) {
        say("readMemory");
        return Super::template readMemory<nBits>(sr, addr, cond);
    }

    template<size_t nBits>
    void writeMemory(X86SegmentRegister sr, const ValueType<32> &addr, const ValueType<nBits> &data,
                     const ValueType<1> &cond) {
        say("writeMemory");
        Super::writeMemory(sr, addr, data, cond);
    }
};
