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
    virtual ~SharedMemoryContext();

    /*------------------------------------------------------------------------------------------------------------
     * Inputs to the callback
     *------------------------------------------------------------------------------------------------------------*/

    /** Architecture on which shared memory access occurs. */
    ArchitecturePtr architecture;

    /** Instruction semantics operators.
     *
     *  This also includes the current state and the SMT solver. */
    InstructionSemantics2::BaseSemantics::RiscOperatorsPtr ops;

    /** Address of instruction accessing the shared memory. */
    rose_addr_t ip;

    /** Address of memory being accessed. */
    rose_addr_t memoryVa;

    /** Number of bytes being accessed. */
    size_t nBytes;

    /** Direction of access. */
    IoDirection direction;

    /*------------------------------------------------------------------------------------------------------------
     * Outputs from the callback
     *------------------------------------------------------------------------------------------------------------*/

    /** Optional byte read.
     *
     *  If a read operation needs to return a special value, then this is the value returned. */
    InstructionSemantics2::BaseSemantics::SValuePtr result;

    /** Optional execution event.
     *
     *  This is to adjust the concrete execution. */
    ExecutionEventPtr event;
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

    /** Callback.
     *
     *  The @p handled argument indicates whether any previous callback has already handled this system call, and if so, this
     *  callback should possibly be a no-op. Returns true if this or any prior callback has handled the system call. */
    virtual bool operator()(bool handled, SharedMemoryContext&) = 0;

    /** Prints callback name and memory information. */
    void hello(const std::string &myName, const SharedMemoryContext&) const;

    /** Create the event that represents the shared memory read and add it to the context. */
    ExecutionEventPtr createReadEvent(SharedMemoryContext&) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shared memory declarations and inter-access data.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Description of shared memory region.
 *
 *  This class describes various things about how a shared memory region behaves. For instance, shared memory representing a
 *  timer is maybe required to be monotonically increasing when read. */
class SharedMemory: public Sawyer::SharedObject {
public:
    /** Reference counting ointer. */
    using Ptr = SharedMemoryPtr;

    /** Callbacks for handling shared memory access. */
    using Callbacks = Sawyer::Callbacks<SharedMemoryCallbackPtr>;

private:
    std::vector<Sawyer::Optional<uint8_t>> prevReadConcrete_; // previous concrete bytes read
    std::vector<SymbolicExpr::Ptr> prevReadSymbolic_;   // previous symbolic bytes read
    Callbacks callbacks_;                               // list of user functions to handle this memory access

protected:
    SharedMemory();

public:
    /** Default allocating constructor. */
    static SharedMemoryPtr instance();

    ~SharedMemory();

public:
    /** Property: Previous concrete byte read.
     *
     *  Holds the optional concrete byte at the specified offset from the beginning of the shared memory region.
     *
     * @{ */
    Sawyer::Optional<uint8_t> previousReadConcreteAtOffset(size_t offset) const;
    void previousReadConcreteAtOffset(size_t offset, const Sawyer::Optional<uint8_t> &byte);
    /** @} */

    /** Property: Previous symbolic byte read.
     *
     *  Holds the optional symbolic byte at the specified offset from the beginning of the shared memory region.
     *
     * @{ */
    SymbolicExpr::Ptr previousReadSymbolicAtOffset(size_t offset) const;
    void previousReadSymbolicAtOffset(size_t offset, const SymbolicExpr::Ptr &byte);
    /** @} */

    /** Property: Callbacks.
     *
     *  List of user-defined functions that could potentially handle this shared memory access.
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
