///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstrates multi-domain semantics with conversions between the values.
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
// The following changes are necessary in order to use the MultiDomainDemoPolicy.
//     In RSIM_SemanticsSettings.h
//         1. Tell the simulator that we'll be using multi-domain semantics by specifying "Paradigm C".
//         2. Set the RSIM_SEMANTICS_POLICY to our MultiDomainDemoPolicy class, using the default state and value type.
//         3. Set sub-domains 1 & 2 to IntervalSemantics and SymbolicSemantics, respectively.
//         4. Include headers for IntervalSemantics, SymbolicSemantics, and this header file.
//     In RSIM_Templates.h
//         5. Include our method definitions from MultiWithConverstionTpl.h
//
// Here's a diff for the two files:
//     diff --git a/projects/simulator/RSIM_SemanticsSettings.h b/projects/simulator/RSIM_SemanticsSettings.h
//     index f959600..fc17504 100644
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
//     @@ -57,7 +57,7 @@
//          // Paradigm C:  Use outer semantics (using inner semantics indirectly, along with additional user semantics)
//      #   define RSIM_SEMANTICS_VTYPE  RSIM_Semantics::OuterValueType
//      #   define RSIM_SEMANTICS_STATE  RSIM_Semantics::OuterState<>
//     -#   define RSIM_SEMANTICS_POLICY RSIM_Semantics::OuterPolicy<>
//     +#   define RSIM_SEMANTICS_POLICY MultiDomainDemoPolicy<RSIM_Semantics::OuterState, RSIM_Semantics::OuterValueType>
//      #endif
//      /** @} */
//      
//     @@ -78,12 +78,12 @@
//      #define RSIM_SEMANTICS_OUTER_0_VTYPE    RSIM_Semantics::InnerValueType
//      #define RSIM_SEMANTICS_OUTER_0_STATE    RSIM_Semantics::InnerState
//      #define RSIM_SEMANTICS_OUTER_0_POLICY   RSIM_Semantics::InnerPolicy
//     -#define RSIM_SEMANTICS_OUTER_1_VTYPE    BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//     -#define RSIM_SEMANTICS_OUTER_1_STATE    BinaryAnalysis::InstructionSemantics::NullSemantics::State
//     -#define RSIM_SEMANTICS_OUTER_1_POLICY   BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
//     -#define RSIM_SEMANTICS_OUTER_2_VTYPE    BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//     -#define RSIM_SEMANTICS_OUTER_2_STATE    BinaryAnalysis::InstructionSemantics::NullSemantics::State
//     -#define RSIM_SEMANTICS_OUTER_2_POLICY   BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
//     +#define RSIM_SEMANTICS_OUTER_1_VTYPE    BinaryAnalysis::InstructionSemantics::IntervalSemantics::ValueType
//     +#define RSIM_SEMANTICS_OUTER_1_STATE    BinaryAnalysis::InstructionSemantics::IntervalSemantics::State
//     +#define RSIM_SEMANTICS_OUTER_1_POLICY   BinaryAnalysis::InstructionSemantics::IntervalSemantics::Policy
//     +#define RSIM_SEMANTICS_OUTER_2_VTYPE    BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType
//     +#define RSIM_SEMANTICS_OUTER_2_STATE    BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State
//     +#define RSIM_SEMANTICS_OUTER_2_POLICY   BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy
//      #define RSIM_SEMANTICS_OUTER_3_VTYPE    BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
//      #define RSIM_SEMANTICS_OUTER_3_STATE    BinaryAnalysis::InstructionSemantics::NullSemantics::State
//      #define RSIM_SEMANTICS_OUTER_3_POLICY   BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
//     @@ -95,8 +95,11 @@
//      /** Include files.
//       *
//       *  All necessary semantics are included here.  This is where the user-defined semantics headers should be included. */
//     +#include "IntervalSemantics.h"
//     +#include "SymbolicSemantics.h"
//      #include "RSIM_SemanticsInner.h"
//      #include "RSIM_SemanticsOuter.h"
//     +#include "demos/MultiWithConversion.h"
//  
//     diff --git a/projects/simulator/RSIM_Templates.h b/projects/simulator/RSIM_Templates.h
//     index 47c722f..2e8d7a9 100644
//     --- a/projects/simulator/RSIM_Templates.h
//     +++ b/projects/simulator/RSIM_Templates.h
//     @@ -393,4 +393,6 @@ RSIM_Semantics::InnerPolicy<State, ValueType>::load_sr_shadow(X86SegmentRegister
//          //ROSE_ASSERT(sr_shadow[sr].present); //checked when used
//      }
//      
//     +#include "demos/MultiWithConversionTpl.h"
//     +
//      #endif /* ROSE_RSIM_Templates_H */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 




// Define the template portion of the MultiDomainDemoPolicy so we don't have to repeat it over and over in the method
// defintions found in MultiWithConversionTpl.h.  This also helps XEmac's c++-mode auto indentation engine since it seems to
// get confused by complex multi-line templates.
#define MULTI_DOMAIN_TEMPLATE template <                                                                                       \
    template <template <size_t> class ValueType> class State,                                                                  \
    template <size_t nBits> class ValueType                                                                                    \
>


MULTI_DOMAIN_TEMPLATE
class MultiDomainDemoPolicy: public RSIM_Semantics::OuterPolicy<State, ValueType> {
public:
    typedef                         RSIM_Semantics::OuterPolicy<State, ValueType> Super;

    // Make names for the sub policies.
    RSIM_SEMANTICS_OUTER_BASE::SP0 CONCRETE;
    RSIM_SEMANTICS_OUTER_BASE::SP1 INTERVAL;
    RSIM_SEMANTICS_OUTER_BASE::SP2 SYMBOLIC;

    // Make simpler names for sub policy value types.  These are class templates. The rhs is defined in
    // RSIM_SemanticsSettings.h. We have to use #define until c++11.
#   define CONCRETE_VALUE RSIM_SEMANTICS_OUTER_0_VTYPE
#   define INTERVAL_VALUE RSIM_SEMANTICS_OUTER_1_VTYPE
#   define SYMBOLIC_VALUE RSIM_SEMANTICS_OUTER_2_VTYPE

    // Shorter type names for policies
    typedef RSIM_SEMANTICS_OUTER_0_POLICY<RSIM_SEMANTICS_OUTER_0_STATE, RSIM_SEMANTICS_OUTER_0_VTYPE> ConcretePolicy;
    typedef RSIM_SEMANTICS_OUTER_1_POLICY<RSIM_SEMANTICS_OUTER_1_STATE, RSIM_SEMANTICS_OUTER_1_VTYPE> IntervalPolicy;
    typedef RSIM_SEMANTICS_OUTER_2_POLICY<RSIM_SEMANTICS_OUTER_2_STATE, RSIM_SEMANTICS_OUTER_2_VTYPE> SymbolicPolicy;

    const char *name;                                   // name to use in diagnostic messages
    bool triggered;                                     // Have we turned on any of our domains yet?
    unsigned allowed_policies;                          // domains that we can allow to be active (after we're triggered)

    // "Inherit" super class' constructors (assuming no c++11)
    MultiDomainDemoPolicy(RSIM_Thread *thread)
        : Super(thread), name(NULL), triggered(false), allowed_policies(0x07) {
        init();
    }

    // Initializer used by constructors.  This is where the SMT solver gets attached to the policy.
    void init();

    // Returns the message stream for the calling threads miscellaneous diagnostics.  We try to always use this for output so
    // that we can turn it on/off via simulator's "--debug" switch, so that output from multiple threads is still readable, and
    // so that all lines are prefixed with information about where/when they were produced. See ROSE's RTS_Message doxygen
    // documentation.
    RTS_Message *trace();

    // Calling this method will cause all our subdomains (the "allowed_policies") to be activated and the simulator will branch
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


};
