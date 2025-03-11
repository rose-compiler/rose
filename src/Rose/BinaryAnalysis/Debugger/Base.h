#ifndef ROSE_BinaryAnalysis_Debugger_Base_H
#define ROSE_BinaryAnalysis_Debugger_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>

#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/Debugger/ThreadId.h>
#include <Rose/Constants.h>

#include <Sawyer/BitVector.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/Trace.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Base class for debuggers.
 *
 *  This defines the interface and common operations for debuggers. */
class Base: public Sawyer::SharedObject {
public:
    /** Shared ownership pointer. */
    using Ptr = Debugger::Ptr;

protected:
    Disassembler::BasePtr disassembler_;                // how to disassemble instructions

protected:
    Base();

    // Debuggers are not copyable
    Base(const Base&) = delete;
    Base(Base&&) = delete;
    Base& operator=(const Base&) = delete;
    Base& operator=(Base&&) = delete;

public:
    virtual ~Base();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Property: Disassembler. */
    virtual Disassembler::BasePtr disassembler();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Process state
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Tests whether this debugger is attached to a specimen.
     *
     *  Returns true if this debugger is debugging something, and false otherwise. */
    virtual bool isAttached() = 0;

    /** Detach from a specimen.
     *
     *  This debugger detaches from the specimen. Depending on the settings, the specimen might continue to run, or it might be
     *  terminated. */
    virtual void detach() = 0;

    /** Terminate the specimen.
     *
     *  If a specimen is attached, terminate it. */
    virtual void terminate() = 0;

    /** Returns true if the subordinate terminated. */
    virtual bool isTerminated() = 0;

    /** String describing how the subordinate process terminated. */
    virtual std::string howTerminated() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Threads
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** List of subordinate threads.
     *
     *  Get the list of thread IDs in the subordinate process. */
    virtual std::vector<ThreadId> threadIds() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Break points
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Set breakpoints. */
    virtual void setBreakPoint(const AddressInterval&) = 0;
    virtual void setBreakPoints(const AddressIntervalSet&) = 0;

    /** Remove breakpoints. */
    virtual void clearBreakPoint(const AddressInterval&) = 0;

    /** Remove all breakpoints. */
    virtual void clearBreakPoints() = 0;

    /** All known breakpoints. */
    virtual AddressIntervalSet breakPoints() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Execution
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Execution address.
     *
     *  This is the value of the so-called program counter. For instance, if the specimen is i386 then this is the value stored
     *  in the EIP register.
     *
     * @{ */
    virtual void executionAddress(ThreadId, Address);
    virtual Address executionAddress(ThreadId);
    /** @} */

    /** Execute one machine instruction. */
    virtual void singleStep(ThreadId) = 0;

    /** Run until the next breakpoint is reached. */
    virtual void runToBreakPoint(ThreadId) = 0;

    /** Run the program and return an execution trace. */
    virtual Sawyer::Container::Trace<Address> trace();

    /** Run the program and return an execution trace.
     *
     *  At each step along the execution, the @p filter functor is invoked and passed the current execution address. The return
     *  value of type @ref FilterAction from the filter functor controls whether the address is appended to the trace and whether the
     *  tracing should continue. */
    template<class Filter>
    Sawyer::Container::Trace<Address> trace(ThreadId tid, Filter &filter) {
        Sawyer::Container::Trace<Address> retval;
        while (!isTerminated()) {
            Address va = executionAddress(tid);
            FilterAction action = filter(va);
            if (action.isClear(FilterActionFlag::REJECT))
                retval.append(va);
            if (action.isSet(FilterActionFlag::STOP))
                return retval;
            singleStep(tid);
        }
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Registers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Register dictionary for the architecture. */
    virtual RegisterDictionaryPtr registerDictionary();

    /** Convert a register descriptor to a register name. */
    virtual std::string registerName(RegisterDescriptor);

    /** Read subordinate register.
     *
     *  Some registers are wider than what can be easily represented on this architecture (e.g., x86 XMM registers are 128
     *  bits), therefore return the result as a bit vector. If you want just the low-order 64 bits, invoke it like this:
     *
     * @code
     *  uint64_t value = debugger->readRegister(RIP).toInteger();
     * @endcode */
    virtual Sawyer::Container::BitVector readRegister(ThreadId, RegisterDescriptor) = 0;

    /** Write subordinate register.
     *
     *  @{ */
    virtual void writeRegister(ThreadId, RegisterDescriptor, const Sawyer::Container::BitVector&) = 0;
    virtual void writeRegister(ThreadId, RegisterDescriptor, uint64_t value) = 0;
    /** @} */

    /** List of available registers. */
    virtual std::vector<RegisterDescriptor> availableRegisters() = 0;

    /** Read all available register values as a single bit vector.
     *
     *  The register values are returned in the same order as described by the @ref availableRegisters method. */
    virtual Sawyer::Container::BitVector readAllRegisters(ThreadId) = 0;

    /** Write all registers as a single bit vector.
     *
     *  The bit vector is the concatenation of the values for all the registers returned by @ref availableRegisters and
     *  in the same order, with no padding. Usually the values are simply the same vector returned by readAllRegisters. */
    virtual void writeAllRegisters(ThreadId, const Sawyer::Container::BitVector&) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Memory
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. */
    virtual size_t readMemory(Address va, size_t nBytes, uint8_t *buffer) = 0;

    /** Read subordinate memory as an array of bytes.
     *
     *  If the read fails then a shorter buffer is returned. */
    virtual std::vector<uint8_t> readMemory(Address va, size_t nBytes) = 0;

    /** Read subordinate memory as a bit vector.
     *
     * @code
     *  uint64_t value = debugger->readMemory(0x12345678, 4, ByteOrder::ORDER_LSB).toInteger();
     * @endcode */
    virtual Sawyer::Container::BitVector readMemory(Address va, size_t nBytes, ByteOrder::Endianness order) = 0;

    /** Writes some bytes to subordinate memory.
     *
     *  Returns the number of bytes written. */
    virtual size_t writeMemory(Address va, size_t nBytes, const uint8_t *bytes) = 0;

    /** Write subordinate memory.
     *
     *  Writes something to memory. */
    template<typename T>
    void writeMemory(Address va, const T &value) {
        size_t n = writeMemory(va, sizeof(T), (const uint8_t*)&value);
        ASSERT_always_require(n == sizeof(T));
    }

    /** Read C-style NUL-terminated string from subordinate.
     *
     *  Reads up to @p maxBytes bytes from the subordinate or until an ASCII NUL character is read, concatenates all the
     *  characters (except the NUL) into a C++ string and returns it. The @p maxBytes includes the NUL terminator although the
     *  NUL terminator is not returned as part of the string. */
    virtual std::string readCString(Address va, size_t maxBytes = UNLIMITED);




};

} // namespace
} // namespace
} // namespace

#endif
#endif
