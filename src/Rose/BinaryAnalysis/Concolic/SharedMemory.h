#ifndef ROSE_BinaryAnalysis_Concolic_SharedMemory_H
#define ROSE_BinaryAnalysis_Concolic_SharedMemory_H
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
// Shared memory context for callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for shared memory context.
 *
 *  This contains information about access to shared memory and is passed to the shared memory callbacks. */
class SharedMemoryContext {
public:
    SharedMemoryContext() = delete;

    /** Constructor for shared memory event replay. */
    SharedMemoryContext(const ArchitecturePtr&, const ExecutionEventPtr &sharedMemoryEvent);

    /** Constructor when a new shared memory event is encountered. */
    SharedMemoryContext(const ArchitecturePtr&, const Emulation::RiscOperatorsPtr&,
                        const ExecutionEventPtr &sharedMemoryEvent);

    virtual ~SharedMemoryContext();

    /*------------------------------------------------------------------------------------------------------------
     * Inputs to the callback
     *------------------------------------------------------------------------------------------------------------*/

    /** Phase of execution.
     *
     *  During the @c REPLAY phase, the callback's @ref SharedMemoryCallback::replay "replay" method is called,
     *  during the @c EMULATION phase, the callback's @ref SharedMemoryCallback::handlePreSharedMemory method is
     *  called, and during the @c POST_EMULATION phase, the callback's @ref SharedMemoryCallback::handlePostSharedMemory
     *  method is called. */
    ConcolicPhase phase = ConcolicPhase::EMULATION;

    /** Architecture on which shared memory access occurs. */
    ArchitecturePtr architecture;

    /** Instruction semantics operators.
     *
     *  This also includes the current state and the SMT solver. */
    Emulation::RiscOperatorsPtr ops;

    /** Address of instruction accessing the shared memory. */
    rose_addr_t ip = 0;

    /** Address of memory being accessed. */
    rose_addr_t memoryVa = 0;

    /** Number of bytes being accessed. */
    size_t nBytes = 0;

    /** Direction of access. */
    IoDirection direction = IoDirection::READ;

    /** Execution event.
     *
     *  This is the introductory event that marks this as a shared memory read. */
    ExecutionEventPtr sharedMemoryEvent;

    /** Optional value read.
     *
     *  If a read operation needs to return a special value, then this is the value returned.
     *
     *  During execution event playback, this is the result read from memory, which is always a concrete value. */
    SymbolicExpr::Ptr valueRead;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks to handle shared memory operations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for shared memory callbacks. */
class SharedMemoryCallback: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = SharedMemoryCallbackPtr;

    virtual ~SharedMemoryCallback() {}

    /** Prints callback name and memory information.
     *
     *  If @p myName is empty, then use the name from the shared memory event in the provided context. */
    void hello(const std::string &myName, const SharedMemoryContext&) const;

    /** Callback for shared memory playback.
     *
     *  This method, which must be implemented in subclasses, is invoked when a shared memory operation event is replayed.
     *  This occurs during the startup phase of concolic testing in order to bring the newly created concrete state up to the
     *  point it should be when the combined concrete plus symbolic phase takes over. Subclasses must define this function. */
    virtual void playback(SharedMemoryContext&) = 0;

    /** Callback for new shared memory events.
     *
     *  These two methods, @ref handlePreSharedMemory and @ref handlePostSharedMemory, are invoked after the startup phase
     *  (handled by @ref playback) each time a new shared memory operation is encountered. By time these are called, a shared
     *  memory @ref ExecutionEvent has already been created and represents the fact that the current instruction operates on
     *  shared memory.
     *
     *  The @ref handlePreSharedMemory is invoked during symbolic instruction emulation before the concrete execution
     *  occurs. It's invoked as soon as the concolic testing semantics realize that a shared memory operation is occuring. The
     *  @ref handlePreSharedMemory methods for all callbacks registered for the specified memory address are invoked before
     *  continuing. Subclasses must define this function, and it should return (via context argument) the value that is read
     *  from memory.
     *
     *  The @ref handlePostSharedMemory is invoked after both symbolic and concrete emulation of the instruction has completed.
     *  The @ref handlePostSharedMemory methods for all callbacks registered for the specified memory address are invoked before
     *  continuing. The default implementation for this method does nothing and is not often needed by subclasses.
     *
     * @{ */
    virtual void handlePreSharedMemory(SharedMemoryContext&) = 0;
    virtual void handlePostSharedMemory(SharedMemoryContext&) {}
    /** @} */

    virtual bool operator()(bool handled, SharedMemoryContext&) final;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
