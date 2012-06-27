// A small example of a multi-domain that shows how to get access to all the state of each semantic domain before and after
// each and every RISC operation performed by the simualtor in the process of simulating a specimen.  The idea is that we
// would define our own semantic domains to run concurrently with the simulator's concrete domain, but we omit that step since
// it's demonstrated by demos/RiscTracingPolicy.h (instead, we just use the three default instances of ROSE's NullSemantics
// domain.
//
// As described in RSIM_SemanticsSettings.h, there are at least two ways to gain control between RISC operations: either define
// a semantic policy that knows about its siblings, or subclass the simulator's RSIM_Semantics::OuterPolicy.  This demo shows
// the latter, since it's a cleaner design.  Namely, we monitor the RISC readRegister() calls and when the one millionth
// attempt (check_readRegister::trigger) is made to read a register then we immediately initialize the EIP register to zero in
// the concrete state.  The simulator's concrete domain is always first in the list of sub-domains, and so is identified by
// RSIM_SEMANTICS_OUTER_BASE::SP0().
//
// In order to make the simulator use our MultiDomainDemoPolicy rather than its own policy, we need to change two lines in
// RSIM_SemanticsSettings.h: the line that says that the simulator should use "Paradigm C" (the multi-semantics) and the line
// that identifies the simulator's semantic policy.  Thus:
//
//     #define RSIM_SEMANTICS_PARADIGM RSIM_SEMANTICS_PARADIGM_C
//
//     #define RSIM_SEMANTICS_POLICY MultiDomainDemoPolicy<RSIM_Semantics::OuterState, RSIM_Semantics::OuterValueType>
//
// We also need to include this header file in RSIM_SemamanticsSettings.h.  Here's a diff for these changes as of 2012-06-13:
//
//     diff --git a/projects/simulator/RSIM_SemanticsSettings.h b/projects/simulator/RSIM_SemanticsSettings.h
//     index f959600..0895f70 100644
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
//     @@ -97,7 +97,7 @@
//       *  All necessary semantics are included here.  This is where the user-defined semantics headers should be included. */
//      #include "RSIM_SemanticsInner.h"
//      #include "RSIM_SemanticsOuter.h"
//     +#include "demos/MultiDomainDemo.h"
//      

template <
    template <template <size_t> class ValueType> class State, // unused
    template <size_t nBits> class ValueType
>
class MultiDomainDemoPolicy: public RSIM_Semantics::OuterPolicy<State, ValueType> {
public:
    typedef RSIM_Semantics::OuterPolicy<State, ValueType> Super;
    uint64_t total_risc_ops;

    MultiDomainDemoPolicy(RSIM_Thread *thread): RSIM_Semantics::OuterPolicy<State, ValueType>(thread), total_risc_ops(0) {}

    // Here's how you get control of a particular RISC operation: you augment the method(s) that implement that operation.
    template<size_t nBits>
    ValueType<nBits> readRegister(const char *regname) {
        check_readRegister();
        return Super::template readRegister<nBits>(regname);
    }
    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
        check_readRegister();
        return Super::template readRegister<nBits>(reg);
    }

    // Here's how you can get control before and/or after every RISC operation: you redefine the before() and/or after()
    // method, which is invoked before the RISC operation happens in any of the sub-domains, or after all the subdomains had a
    // chance to process the RISC operation.
    void after() {
        ++total_risc_ops;
    }

protected:
    void
    check_readRegister() {
        static const uint64_t trigger = 1000000;
        static uint64_t ncalls = 0;
        if (ncalls++ == trigger) {
            std::cerr <<"MultiDomainDemoPolicy: triggered before readRegister call #" <<trigger <<"\n"
                      <<"    this is RISC operation #" <<total_risc_ops <<"\n"
                      <<"    setting concrete EIP value to null to cause a specimen segmentation fault...\n";
            RSIM_SEMANTICS_OUTER_0_VTYPE<32> null(0);
#if 0
            // This is one of many ways to do this.
            RSIM_SEMANTICS_OUTER_0_STATE<> &concrete_state = this->get_concrete_state();
            concrete_state.gpr[this->reg_eip.get_major()] = null;
#else
            // Another way that doesn't need to know about how the simulator's concrete state is organized
            this->get_policy(RSIM_SEMANTICS_OUTER_BASE::SP0()).writeRegister("eip", null);
#endif
        }
    }
};


        

        

            
