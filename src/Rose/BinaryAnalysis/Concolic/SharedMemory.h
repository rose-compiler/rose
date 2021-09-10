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
     *  During the playback phase, is the concrete value (as a symbolic expression) to be substituted as the result of the
     *  shared memory read. However, if the read was originally treated as non-shared (implying also that there is no input
     *  variable) then this data member is a null pointer. */
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
    virtual void handlePostSharedMemory(const SharedMemoryContext&) {}
    /** @} */

    /** Treat a shared memory read as normal memory.
     *
     *  This cancels any special value returned by the shared memory read. The context @ref SharedMemoryContext::valueRead
     *  "valueRead" member is set to null and the input variable is discarded and cleared from the shared memory read event.
     *  This can only be called from the @ref handlePreSharedMemory function. */
    void normalRead(SharedMemoryContext&) const;

    /** Do not treat this read as test case input.
     *
     *  Calling this method marks this memory read as not being a configurable input for future test cases. However, it is still
     *  possible to treat this read as shared memory returning a value that does not follow normal memory semantics. */
    void notAnInput(SharedMemoryContext&) const;

    /** Input variable for future test cases.
     *
     *  Returns the input variable used to adjust future test cases. This is probably the variable you want to constrain in
     *  various ways according to the semantics of the shared memory.  Returns null if this read is not being treated as adjustable
     *  input in future test cases (see @ref notAnInput). */
    SymbolicExprPtr inputVariable(const SharedMemoryContext&) const;


    virtual bool operator()(bool handled, SharedMemoryContext&) final;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
