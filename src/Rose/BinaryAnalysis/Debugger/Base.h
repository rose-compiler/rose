#ifndef ROSE_BinaryAnalysis_Debugger_Base_H
#define ROSE_BinaryAnalysis_Debugger_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>

#include <Rose/BinaryAnalysis/Debugger/ThreadId.h>

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
    Base();

    // Debuggers are not copyable
    Base(const Base&) = delete;
    Base(Base&&) = delete;
    Base& operator=(const Base&) = delete;
    Base& operator=(Base&&) = delete;

public:
    virtual ~Base();

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

    /** List of subordinate threads.
     *
     *  Get the list of thread IDs in the subordinate process. */
    virtual std::vector<ThreadId> threadIds() = 0;

    /** Execution address.
     *
     *  This is the value of the so-called program counter. For instance, if the specimen is i386 then this is the value stored
     *  in the EIP register.
     *
     * @{ */
    virtual void executionAddress(ThreadId, rose_addr_t) = 0;
    virtual rose_addr_t executionAddress(ThreadId) = 0;
    /** @} */

    /** Set breakpoints. */
    virtual void setBreakPoint(const AddressInterval&) = 0;

    /** Remove breakpoints. */
    virtual void clearBreakPoint(const AddressInterval&) = 0;

    /** Remove all breakpoints. */
    virtual void clearBreakPoints() = 0;

    /** Execute one machine instruction. */
    virtual void singleStep(ThreadId) = 0;

    /** Run until the next breakpoint is reached. */
    virtual void runToBreakPoint(ThreadId) = 0;

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

    /** Read subordinate memory.
     *
     *  Returns the number of bytes read. */
    virtual size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer) = 0;

    /** Read subordinate memory as an array of bytes.
     *
     *  If the read fails then a shorter buffer is returned. */
    virtual std::vector<uint8_t> readMemory(rose_addr_t va, size_t nBytes) = 0;

    /** Read subordinate memory as a bit vector.
     *
     * @code
     *  uint64_t value = debugger->readMemory(0x12345678, 4, ByteOrder::ORDER_LSB).toInteger();
     * @endcode */
    virtual Sawyer::Container::BitVector readMemory(rose_addr_t va, size_t nBytes, ByteOrder::Endianness order) = 0;

    /** Writes some bytes to subordinate memory.
     *
     *  Returns the number of bytes written. */
    virtual size_t writeMemory(rose_addr_t va, size_t nBytes, const uint8_t *bytes) = 0;

    /** Write subordinate memory.
     *
     *  Writes something to memory. */
    template<typename T>
    void writeMemory(rose_addr_t va, const T &value) {
        size_t n = writeMemory(va, sizeof(T), (const uint8_t*)&value);
        ASSERT_always_require(n == sizeof(T));
    }

    /** Read C-style NUL-terminated string from subordinate.
     *
     *  Reads up to @p maxBytes bytes from the subordinate or until an ASCII NUL character is read, concatenates all the
     *  characters (except the NUL) into a C++ string and returns it. The @p maxBytes includes the NUL terminator although the
     *  NUL terminator is not returned as part of the string. */
    virtual std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED) = 0;

    /** Returns true if the subordinate terminated. */
    virtual bool isTerminated() = 0;

    /** String describing how the subordinate process terminated. */
    virtual std::string howTerminated() = 0;

    /** Available registers. */
    virtual RegisterDictionaryPtr registerDictionary() const = 0;

    /** Disassembler. */
    virtual Disassembler::BasePtr disassembler() const = 0;

    /** Run the program and return an execution trace. */
    virtual Sawyer::Container::Trace<rose_addr_t> trace();

    /** Run the program and return an execution trace.
     *
     *  At each step along the execution, the @p filter functor is invoked and passed the current execution address. The return
     *  value of type @ref FilterAction from the filter functor controls whether the address is appended to the trace and whether the
     *  tracing should continue. */
    template<class Filter>
    Sawyer::Container::Trace<rose_addr_t> trace(ThreadId tid, Filter &filter) {
        Sawyer::Container::Trace<rose_addr_t> retval;
        while (!isTerminated()) {
            rose_addr_t va = executionAddress(tid);
            FilterAction action = filter(va);
            if (action.isClear(FilterActionFlag::REJECT))
                retval.append(va);
            if (action.isSet(FilterActionFlag::STOP))
                return retval;
            singleStep(tid);
        }
        return retval;
    }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
