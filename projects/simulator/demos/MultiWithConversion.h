///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstrates multi-domain semantics with conversions between the values.                             __THIS_HEADER_IS_USED__
// RobinTpl.h contains the functions related to domain selection and conversion.
//
// The basic idea is to make some minor changes to the simulator source code (mostly in RSIM_SemanticsSettings.h) so that the
// simulator will use a semantic domain that we define in this file.  Our semantic domain is ROSE's MultiSemantics (see ROSE
// doxygen documentation) with the following sub-domains:
//     0. the simulator's usual concrete semantics (RSIM_SemanticsInner)
//     1. interval semantics
//     2. symbolic semantics
//     3. null semantics
//
// Our multi-domain semantics class is named MultiDomainDemoPolicy and inherits from RSIM_Semantics::OuterPolicy, which in turn
// inherits from ROSE's MultiSemantics.  The simulator's OuterPolicy class extends MultiSemantics in such a way as to make
// OuterPolicy and InnerPolicy have the same interface, at least to the extent that they're used by the simulator proper.
//
// The MultiDomainDemoPolicy's constructor constructs the sub-domains but disables all of them except the simulator's concrete
// domain.  This allows the simulator to run unimpeded until we "trigger" the MultiDomainDemoPolicy via its trigger()
// method. Once the policy is triggered, our subdomains are enabled and start participating in the simulator's instruction
// semantics calls.  The triggering is controlled by the SemanticController instruction callback defined in
// MultiWithConversion.C, which is a typical, straightforward instruction callback.
//
// Most of the methods for MultiDomainDemoPolicy are defined in MultiWithConversionTpl.h, which is included at the
// end of RSIM_Templates.h. We have to do it this way because some of the simulator's higher-level data structures (like
// RSIM_Thread) are only partially defined here.
//
// The following changes are necessary in order to use the MultiDomainDemoPolicy (see MultiWithConversion.patch for a patch)
//     In RSIM_SemanticsSettings.h
//         1. Tell the simulator that we'll be using multi-domain semantics by specifying "Paradigm C".
//         2. Set the RSIM_SEMANTICS_POLICY to our MultiDomainDemoPolicy class, using the default state and value type.
//         3. Set sub-domains 1 & 2 to IntervalSemantics and SymbolicSemantics, respectively.
//         4. Include headers for IntervalSemantics, SymbolicSemantics, and this header file.
//     In RSIM_Templates.h
//         5. Include our method definitions from MultiWithConverstionTpl.h
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RSIM_MultiWithConversion_H
#define RSIM_MultiWithConversion_H              /* used by MultiWithConversion.C */

#if 0 // used to fool scripts/policies/UnusedHeaders
#include "MultiWithConversion.h"
#include "MultiWithConversionTpl.h"
#endif

namespace MultiDomainDemo {

// Make names for the sub policies.
static const RSIM_SEMANTICS_OUTER_BASE::SP0 CONCRETE = RSIM_SEMANTICS_OUTER_BASE::SP0();
static const RSIM_SEMANTICS_OUTER_BASE::SP1 INTERVAL = RSIM_SEMANTICS_OUTER_BASE::SP1();
static const RSIM_SEMANTICS_OUTER_BASE::SP2 SYMBOLIC = RSIM_SEMANTICS_OUTER_BASE::SP2();

// Make simpler names for sub policy value types.  These are class templates. The rhs is defined in
// RSIM_SemanticsSettings.h. We have to use #define until c++11.
#define CONCRETE_VALUE RSIM_SEMANTICS_OUTER_0_VTYPE
#define INTERVAL_VALUE RSIM_SEMANTICS_OUTER_1_VTYPE
#define SYMBOLIC_VALUE RSIM_SEMANTICS_OUTER_2_VTYPE

// Shorter type names for policies
typedef RSIM_SEMANTICS_OUTER_0_POLICY<RSIM_SEMANTICS_OUTER_0_STATE, RSIM_SEMANTICS_OUTER_0_VTYPE> ConcretePolicy;
typedef RSIM_SEMANTICS_OUTER_1_POLICY<RSIM_SEMANTICS_OUTER_1_STATE, RSIM_SEMANTICS_OUTER_1_VTYPE> IntervalPolicy;
typedef RSIM_SEMANTICS_OUTER_2_POLICY<RSIM_SEMANTICS_OUTER_2_STATE, RSIM_SEMANTICS_OUTER_2_VTYPE> SymbolicPolicy;

// Functions that convert a value from one domain to another.
template <template <size_t> class ValueType, size_t nBits>
CONCRETE_VALUE<nBits> convert_to_concrete(const ValueType<nBits> &value);
template <template <size_t> class ValueType, size_t nBits>
INTERVAL_VALUE<nBits> convert_to_interval(const ValueType<nBits> &value);
template <template <size_t> class ValueType, size_t nBits>
SYMBOLIC_VALUE<nBits> convert_to_symbolic(const ValueType<nBits> &value);

class Exception {
public:
    Exception(const std::string &mesg): mesg(mesg) {}
    std::string mesg;
};

// Mixed-interpretation memory.  Addresses are symbolic expressions and values are multi-domain.  We'll override the
// multi-domain policy's memory access functions to use this state rather than chaining to the memory states associated with
// the sub-policies.
template <template <size_t> class ValueType>
class State: public RSIM_Semantics::OuterState<ValueType> {
public:
    typedef std::pair<SYMBOLIC_VALUE<32>, ValueType<8> > MemoryCell;
    typedef std::list<MemoryCell> MemoryCells;                  // list of memory cells in reverse chronological order
    MemoryCells stack_cells;                                    // memory state for stack memory (accessed via SS register)
    MemoryCells data_cells;                                     // memory state for anything that non-stack (e.g., DS register)
    BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterStateX86<RSIM_SEMANTICS_VTYPE> registers;

    // Write a single byte to memory
    void mem_write_byte(X86SegmentRegister sr, const SYMBOLIC_VALUE<32> &addr, const ValueType<8> &value);

    // Read a single byte from memory.  The active_policies is the bit mask of sub-policies that are currently active. The
    // optional SMT solver is used to prove hypotheses about symbolic expressions (like memory addresses).
    ValueType<8> mem_read_byte(X86SegmentRegister sr, const SYMBOLIC_VALUE<32> &addr, unsigned active_policies, SMTSolver *solver);

    // Returns true if two memory addresses can be equal.
    static bool may_alias(const SYMBOLIC_VALUE<32> &addr1, const SYMBOLIC_VALUE<32> &addr2, SMTSolver *solver);

    // Returns true if two memory address are equivalent.
    static bool must_alias(const SYMBOLIC_VALUE<32> &addr1, const SYMBOLIC_VALUE<32> &addr2, SMTSolver *solver);

    // Printing
    template<size_t nBits> void show_value(std::ostream&, const std::string &hdg, const ValueType<nBits>&, unsigned domains) const;
    void print(std::ostream&, unsigned domain_mask=0x07) const;
    friend std::ostream& operator<<(std::ostream &o, const State &state) {
        state.print(o);
        return o;
    }
};

// Define the template portion of the MultiDomainDemo::Policy so we don't have to repeat it over and over in the method
// defintions found in MultiWithConversionTpl.h.  This also helps XEmac's c++-mode auto indentation engine since it seems to
// get confused by complex multi-line templates.
#define MULTI_DOMAIN_TEMPLATE template <                                                                                       \
    template <template <size_t> class ValueType> class State,                                                                  \
    template <size_t nBits> class ValueType                                                                                    \
>


MULTI_DOMAIN_TEMPLATE
class Policy: public RSIM_Semantics::OuterPolicy<State, ValueType> {
public:
    typedef                         RSIM_Semantics::OuterPolicy<State, ValueType> Super;

    const char *name;                                   // name to use in diagnostic messages
    bool triggered;                                     // Have we turned on any of our domains yet?
    State<ValueType> state;                             // the mixed-semantic state (symbolic address, multi-value)
    unsigned active_policies;                           // Policies that should be active *during* an instruction

    // "Inherit" super class' constructors (assuming no c++11)
    Policy(RSIM_Thread *thread)
        : Super(thread), name(NULL), triggered(false), active_policies(0x07) {
        init();
    }

    // Initializer used by constructors.  This is where the SMT solver gets attached to the policy.
    void init();

    // Returns the message stream for the calling threads miscellaneous diagnostics.  We try to always use this for output so
    // that we can turn it on/off via simulator's "--debug" switch, so that output from multiple threads is still readable, and
    // so that all lines are prefixed with information about where/when they were produced. See ROSE's RTS_Message doxygen
    // documentation.
    RTS_Message *trace();

    // Calling this method will cause all our subdomains to be activated and the simulator will branch
    // to the specified target_va.
    void trigger(rose_addr_t target_va);

    // We can get control at the beginning of every instruction.  This allows us to do things like enabling/disabling
    // sub-domains based on the kind of instruction.  We could also examine the entire multi-domain state at this point and do
    // something based on that.
    void startInstruction(SgAsmInstruction *insn) /*override*/;

    // We can get control at the end of most instructions by hooking into finishInstruction.  But be warned: any instruction
    // that throws an exception will skip over this.  The simulator's concrete semantics throw various exceptions for things
    // like signal handling, thread termination, specimen segmentation faults, etc.
    void finishInstruction(SgAsmInstruction *insn) /*override*/;

    // You can get control around individual RISC operations by augmenting the particular function.  For instance, here's how
    // you would intercept an XOR operation.
    template<size_t nBits>
    ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b);

    // Functor that counts nodes in a symbolic expression tree.
    struct ExprNodeCounter: public InsnSemanticsExpr::Visitor {
        size_t nnodes;
        ExprNodeCounter(): nnodes(0) {}
        virtual void operator()(const InsnSemanticsExpr::TreeNodePtr&) { ++nnodes; }
    };

    // Calls the node counter for a particular value type.  T must be based on SymbolicSemantics::ValueType<>.
    template<class T>
    void symbolic_expr_complexity(const T &value, ExprNodeCounter *visitor) {
        value.get_expression()->depth_first_visit(visitor);
    }

    // Counts nodes across all symbolic registers.  Note that for x86, some registers have multiple names depending on the part
    // of the physical register being referenced.  Therefore, by counting the complexity of each named register, we're counting
    // most physical registers multiple times.  That's fine, as long as we're consistent.
    size_t symbolic_state_complexity();

    // We want to do something special with memory in order to implement mixed semantics.  Therefore, we override the
    // readMemory() and writeMemory() methods.
    template<size_t nBits>
    ValueType<nBits> readMemory(X86SegmentRegister sr, ValueType<32> addr, const ValueType<1> &cond);
    template<size_t nBits>
    void writeMemory(X86SegmentRegister sr, ValueType<32> addr, const ValueType<nBits> &data, const ValueType<1> &cond);

    // We need to hook into register access because we might need to convert values from one domain to another.
    template<size_t nBits>
    ValueType<nBits> readRegister(const char *regname);
    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg);
    template<size_t nBits>
    void writeRegister(const char *regname, const ValueType<nBits> &val);
    template<size_t nBits>
    void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &val);

    // Print the states for each sub-domain and our own state containing the mixed semantics memory.
    void print(std::ostream&, bool abbreviated=false) const;
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o);
        return o;
    }

};
    
} // namespace

#endif
