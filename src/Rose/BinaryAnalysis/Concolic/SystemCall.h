#ifndef ROSE_BinaryAnalysis_Concolic_SystemCall_H
#define ROSE_BinaryAnalysis_Concolic_SystemCall_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>

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
    virtual ~SyscallContext();

    /** Architecture on which system call occurs. */
    ArchitecturePtr architecture;

    /** System call declaration and saved state.
     *
     *  This is the pointer to the object to which this callback is attached. It contains information about the
     *  the system call declaration as well as any state that needs to be saved across calls to this system call. */
    SystemCallPtr systemCall;

    /** Instruction semantics operators.
     *
     *  This also includes the current state and the SMT solver. */
    InstructionSemantics2::BaseSemantics::RiscOperatorsPtr ops;

    /** Address of system call.
     *
     *  Virtual address where the system call occurs. */
    rose_addr_t callSite;

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

    /** Optional return value event.
     *
     *  If a system call returns, then one of the callbacks should create an execution event that describes the return
     *  value, and assign it to this data member. */
    ExecutionEventPtr retEvent;

    /** Optional return input variable.
     *
     *  If the system call return value should be a program input, then a variable should be created to represent that input. The
     *  variable should be saved in this data member.  Any constraints for this variable should be added to the SMT solver
     *  that can be found from the @c ops data member of this object. */
    InstructionSemantics2::BaseSemantics::SValuePtr retSValue;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks to handle system calls.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for system call callbacks. */
class SyscallCallback: public Sawyer::SharedObject {
public:
    using Ptr = SyscallCallbackPtr;

    virtual ~SyscallCallback() {}

    /** Callback.
     *
     *  The @p handled argument indicates whether any previous callback has already handled this system call, and if so, this
     *  callback should possibly be a no-op.  Returns true if this or any prior callback has handled the system call. */
    virtual bool operator()(bool handled, SyscallContext&) const = 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System call declarations and inter-call data.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of system calls.
 *
 *  This class describes various things about system calls. For instance, a system call like SYS_getpid should return the same
 *  value every time it's called since a process ID is constant for the life of the process. */
class SystemCall: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = SystemCallPtr;

    /** Callbacks for handling system call. */
    using Callbacks = Sawyer::Callbacks<SyscallCallbackPtr>;

private:
    Sawyer::Optional<uint64_t> prevReturnConcrete_;     // Previous concrete return value, or nothing
    SymbolicExpr::Ptr prevReturnSymbolic_;              // Previous symbolic return value, or null
    Callbacks callbacks_;                               // List of user functions to handle this syscall

protected:
    SystemCall();

public:
    /** Default allocating constructor. */
    static SystemCallPtr instance();

    ~SystemCall();

public:
    /** Property: Previous concrete return value.
     *
     * @{ */
    const Sawyer::Optional<uint64_t>& previousReturnConcrete() const;
    void previousReturnConcrete(uint64_t);
    /** @} */

    /** Property: Previous symbolic return value.
     *
     * @{ */
    SymbolicExpr::Ptr previousReturnSymbolic() const;
    void previousReturnSymbolic(const SymbolicExpr::Ptr&);
    /** @} */

    /** Property: Callbacks.
     *
     *  List of user-defined functions that could potentially handle this system call.
     *
     * @{ */
    const Callbacks& callbacks() const;
    Callbacks& callbacks();
    /** @} */
};

} // namespace
} // namespace
} // namespace

#endif
#endif
