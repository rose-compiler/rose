#ifndef ROSE_RSIM_SemanticsSettings_H
#define ROSE_RSIM_SemanticsSettings_H

/******************************************************************************************************************************
 *                                      User-level Semantic Info
 *
 * This file is where users can define their own semantics that will execute concurrently with the concrete semantics used by
 * the simulator.  The simulator supports at least three paradigms for user-level semantic analysis:
 *
 * Paradigm A: The simulator runs using concrete semantics until a certain point (such as the program original entry point or
 *    the start of the "main" function), and then the user instantiates his own semantics and does his own analysis while the
 *    simulator is paused.
 *
 * Paradigm B. The user instantiates his semantics and registers an instruction-level callback.  Every time the simulator is
 *    about to execute an instruction in the concrete domain, the user first executes the instruction in his own semantic
 *    domains, possibly using parts of the simulators current concrete state. He then has a chance to modify the simulator's
 *    concrete state to influence simulator behavior (such as setting the EIP register to a new instruction address).
 *
 * Paradigm C. The user instantiates his semantics when the simulator is created, combining them with the simulator's concrete
 *    semantics via ROSE MultiSemantics object.  When the simulator executes an instruction, the instruction is broken down
 *    into its constituent RISC operations and each of those operations is invoked in the concrete domain and the user's
 *    domain(s).  The user has an opportunity to interact with all the domains' states between each RISC operation.  Thus
 *    Paradigm C has a finer granularity of interaction than Paradigm B.
 *
 * The simulator uses a concrete domain for Paradigms A and B; it uses a multi-domain having a child concrete domain (and
 * user-defined domains) for Paradigm C.  This is described more in the documentation for the RSIM_Semantics namespace.
 ******************************************************************************************************************************/


#define RSIM_SEMANTICS_PARADIGM_A       0                       /* Paradigm A: Simulator uses "Inner"(-like) semantics */
#define RSIM_SEMANTICS_PARADIGM_B       1                       /* Paradigm B: Simulator uses "Inner"(-like) semantics */
#define RSIM_SEMANTICS_PARADIGM_C       2                       /* Paradigm C: Simulator uses "Outer"(-like) semantics */

#define RSIM_SEMANTICS_PARADIGM RSIM_SEMANTICS_PARADIGM_A       /* The paradigm we are actually using */


/** ValueType, State, and Semantic Policy used throughout the simulator.
 *
 *  Use the "Inner" classes for Paradigms A and B.  "Inner" classes implement the simulator's concrete semantics by deriving
 *  from ROSE's PartialSymbolicSemantics domain (the simulator uses only the concrete capabilities of that class).  You may
 *  replace "Inner" with an identical interface if you need to do something just a little different in the concrete domain than
 *  the simulator already does.
 *
 *  Use the "Outer" classes for Paradigm C.  "Outer" classes implement multi-semantics by deriving from ROSE's MultiSemantics
 *  domain.  The first subdomain of which is defined to be the simulator's concrete domain (the "Inner" classes).  The user
 *  will want to modify the RSIM_SEMANTICS_OUTER_i_{VTYPE,STATE,POLICY} symbols where i>0 so they name his own semantic classes
 *  rather than ROSE's NullSemantics.  The "Outer" classes can be replaced by any other classes that have an identical
 *  interface.
 *
 * @{ */
#if RSIM_SEMANTICS_PARADIGM==RSIM_SEMANTICS_PARADIGM_A || RSIM_SEMANTICS_PARADIGM==RSIM_SEMANTICS_PARADIGM_B
    // Paradigms A and B:  Use inner semantics directly
#   define RSIM_SEMANTICS_VTYPE  RSIM_Semantics::InnerValueType
#   define RSIM_SEMANTICS_STATE  RSIM_Semantics::InnerState<>
#   define RSIM_SEMANTICS_POLICY RSIM_Semantics::InnerPolicy<>
#else
    // Paradigm C:  Use outer semantics (using inner semantics indirectly, along with additional user semantics)
#   define RSIM_SEMANTICS_VTYPE  RSIM_Semantics::OuterValueType
#   define RSIM_SEMANTICS_STATE  RSIM_Semantics::OuterState<>
#   define RSIM_SEMANTICS_POLICY RSIM_Semantics::OuterPolicy<>
#endif
/** @} */




/** Paradigm C sub-domains.
 *
 *  Subdomain 0 must be the simulator concrete domain (or a user-defined version with an identical interface).  The user may
 *  modify subdomains 1 through 3 to be whatever they like.  See the rose::BinaryAnalysis::InstructionSemantics::MultiSemantics
 *  class in ROSE's API Reference Manual for details.
 *
 *  There are at least two ways for the user to get control between RISC operations in order to have an opportunity to
 *  manipulate the state of all the sub-domains. One is to provide an implementation for startInstruction() and/or
 *  finishInstruction() in a subdomain that knows about its sibling subdomains.  The other approach is to subclass
 *  RSIM_Semantics::OuterPolicy and augment its startInstruction() and/or endInstruction().  The second approach is cleaner,
 *  but the simulator needs to be informed of the new class by defining RSIM_SEMANTICS_POLICY above. */
#define RSIM_SEMANTICS_OUTER_0_VTYPE    RSIM_Semantics::InnerValueType
#define RSIM_SEMANTICS_OUTER_0_STATE    RSIM_Semantics::InnerState
#define RSIM_SEMANTICS_OUTER_0_POLICY   RSIM_Semantics::InnerPolicy
#define RSIM_SEMANTICS_OUTER_1_VTYPE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#define RSIM_SEMANTICS_OUTER_1_STATE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::State
#define RSIM_SEMANTICS_OUTER_1_POLICY   rose::BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
#define RSIM_SEMANTICS_OUTER_2_VTYPE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#define RSIM_SEMANTICS_OUTER_2_STATE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::State
#define RSIM_SEMANTICS_OUTER_2_POLICY   rose::BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
#define RSIM_SEMANTICS_OUTER_3_VTYPE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#define RSIM_SEMANTICS_OUTER_3_STATE    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::State
#define RSIM_SEMANTICS_OUTER_3_POLICY   rose::BinaryAnalysis::InstructionSemantics::NullSemantics::Policy





/** Include files.
 *
 *  All necessary semantics are included here.  This is where the user-defined semantics headers should be included. */
#include "RSIM_SemanticsInner.h"
#include "RSIM_SemanticsOuter.h"



/** Semantic dispatcher type.  This is the class that decodes x86 instructions and dispatches RISC operations to the various
 *  instruction semantics policies. */
namespace RSIM_Semantics {
typedef rose::BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<RSIM_SEMANTICS_POLICY, RSIM_SEMANTICS_VTYPE> Dispatcher;
}

#endif
