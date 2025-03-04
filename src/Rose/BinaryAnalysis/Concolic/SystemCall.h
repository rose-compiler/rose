#ifndef ROSE_BinaryAnalysis_Concolic_SystemCall_H
#define ROSE_BinaryAnalysis_Concolic_SystemCall_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <Sawyer/Optional.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System call context for callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for system call context.
 *
 *  This contains information about the system call and is passed to the system call callbacks. */
class SyscallContext {
public:
    SyscallContext() = delete;

    /** Constructor for a syscall event to be replayed. */
    SyscallContext(const ArchitecturePtr&, const ExecutionEventPtr &syscallEvent,
                   const std::vector<ExecutionEventPtr> &relatedEvents);

    /** Constructor for first-time system call. */
    SyscallContext(const ArchitecturePtr&, const Emulation::RiscOperatorsPtr&,
                   const ExecutionEventPtr &syscallEvent);

    virtual ~SyscallContext();

    /*------------------------------------------------------------------------------------------------------------
     * Inputs to the callback
     *------------------------------------------------------------------------------------------------------------*/

    /** Phase of execution.
     *
     *
     *  During the @c REPLAY phase, the callback's @ref I386Linux::SyscallBase::playback "playback" method is called, and during the
     *  @c EMULATION phase, the callback's @ref I386Linux::SyscallBase::handlePreSyscall "handlePreSyscall" and @ref
     *  I386Linux::SyscallBase::handlePostSyscall "handlePostSyscall" methods are called. */
    ConcolicPhase phase = ConcolicPhase::EMULATION;

    /** Architecture on which system call occurs. */
    ArchitecturePtr architecture;

    /** Instruction semantics operators.
     *
     *  This also includes the current state and the SMT solver. */
    Emulation::RiscOperatorsPtr ops;

    /** Address of system call.
     *
     *  Virtual address where the system call occurs. */
    Address ip = 0;

    /** Concrete arguments.
     *
     *  The concrete arguments that are available for this system call, regardless of whether the system call actually
     *  uses them. For instance, Linux i386 system calls can have up to six arguments, so on that architecture this vector
     *  will contain all six possible arguments even if the system call is @c getpid, which uses none of them. */
    std::vector<uint64_t> argsConcrete;

    /** Starting event for system call.
     *
     *  Every system call will have an initial execution event that marks the following execution events as side effects of of
     *  this system call. This starting event contains the system call function number, but side effects such as the return
     *  value are described by subsequent events. */
    ExecutionEventPtr syscallEvent;

    /*------------------------------------------------------------------------------------------------------------
     * Inputs and outputs
     *------------------------------------------------------------------------------------------------------------*/

    /** Related events.
     *
     *  During playback (i.e., concrete execution), this is the list of following events for the same instruction. For
     *  instance, there's usually a write-register event that adjusts the system call return value according to an input
     *  variable. These events are also replayed individually, but are provided in the system call context because they might
     *  be needed by the system call callback.
     *
     *  During non-playback (i.e., concolic execution) this list is initially empty. As callbacks are called which might create
     *  additional events, those events should be added to this list. This accomplishes two things: it makes the events easily
     *  obtainable by subsequent callbacks (for use and/or modification), and it ensures that the final state of those events
     *  gets written to the database. */
    std::vector<ExecutionEventPtr> relatedEvents;

    /** System call return event.
     *
     *  During playback (i.e., concrete execution) this property is initially null, although all related events to this
     *  callback are in the @p relatedEvents property and can be searched. If a callback searches @p relatedEvents in order
     *  locate the system call return side effect, it may (at its discretion) set this @p returnEvent property to point
     *  to the correct event.
     *
     *  During non-playback (i.e., concolic execution) this property is initially null because the concrete system call has not
     *  yet been executed. Once it has been executed, the callback that performed the concrete execute step *must* set this to
     *  a non-null value and should also push that event onto the @p relatedEvents list. */
    ExecutionEventPtr returnEvent;

    /** Optional return input variable.
     *
     *  If the system call return value should be a program input, then a variable should be created to represent that input. The
     *  variable should be saved in this data member.  Any constraints for this variable should be added to the SMT solver
     *  that can be found from the @c ops data member of this context object. */
    SymbolicExpression::Ptr symbolicReturn;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks to handle system calls.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for system call callbacks. */
class SyscallCallback: public Sawyer::SharedObject {
public:
    using Ptr = SyscallCallbackPtr;

public:
    virtual ~SyscallCallback() {}

    /** Callback.
     *
     *  The @p handled argument indicates whether any previous callback has already handled this system call, and if so, this
     *  callback should possibly be a no-op.  Returns true if this or any prior callback has handled the system call. */
    virtual bool operator()(bool handled, SyscallContext&) = 0;

    /** Prints callback name and information.
     *
     *  If @p myName is empty, then use the name from the system call event in the provided context. */
    void hello(const std::string &myName, const SyscallContext&) const;

    /** Make the specified event not-an-input. */
    void notAnInput(SyscallContext&, const ExecutionEventPtr&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
