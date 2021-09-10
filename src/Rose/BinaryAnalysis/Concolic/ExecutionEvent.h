#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionLocation.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <Combinatorics.h>                              // rose

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Execution event.
 *
 *  An execution event is some event that happens during concrete execution before and/or after which the execution must be
 *  fixed up. For instance, when executing a Linux program that we want to replay later as a different test case, we need to
 *  capture read system calls, figure out what was read, and save that as an event. That way, when we replay the program later
 *  we can make the read return the same values (or perhaps an adjusted value in order to follow a different execution path).
 *
 *  Furthermore, the interface between the program and the operating system may need to be modeled. For instance, if we're
 *  replaying a program whose getpid system call returned 123 and then we continue to run beyond the recorded events and
 *  encounter a new getpid system call, we need to make sure it also returns 123.  The modeling of the operating system is
 *  beyond the scope of this class, but this class needs to be able to provide all the information to make that modeling
 *  possible. */
class ExecutionEvent: public Sawyer::SharedObject, public Sawyer::SharedFromThis<ExecutionEvent> {
public:
    /** Reference-counting pointer to @ref ExecutionEvent. */
    using Ptr = ExecutionEventPtr;

    /** Type of events. */
    enum class Action {
        NONE,                                           // Don't perform any action. Often just to force execution

        // Actions that have an effect on the simulated process.
        MAP_MEMORY,                                     // Add addresses to the memory map
        UNMAP_MEMORY,                                   // Remove addresses from the memory map
        WRITE_MEMORY,                                   // Change values in the memory map
        HASH_MEMORY,                                    // Verify that a memory region hashes to a particular value
        WRITE_REGISTER,                                 // Change value of a single register
        RESTORE_REGISTERS,                              // Set all register values

        // System calls. The scalar value is the system call number and the bytes hold the system call concrete
        // arguments. Additional events may follow in order to reproduce the effects of the system call.
        OS_SYSCALL,

        // Shared memory reads. The scalar value is the memory address, and additional events may follow in order
        // to reproduce the effect of the read. This is because we can't affect the read by first writing to the address
        // since (1) execution events are replayed after the instruction is executed concretely, and (2) the shared
        // memory at that address might not follow typical memory semantics--a read might not return the value just
        // written, and (3) the memory might not be writable.
        OS_SHM_READ,
    };

private:
    //     !!!!!!! DONT FORGET TO UPDATE ExecutionEvent::copy !!!!!!!!!
    std::string timestamp_;                             // time of creation, needed by the database
    TestCasePtr testCase_;                              // each event belongs to a particular test case

    // Event identification
    ExecutionLocation location_;                        // location event occurs
    size_t instructionPointer_ = 0;                     // value of instruction pointer when event occurs
    std::string name_;                                  // optional name for debugging

    // Association with a symbolic input variable
    InputType inputType_ = InputType::NONE;             // type of input
    SymbolicExpr::Ptr inputVariable_;                   // associated symbolic variable, if any
    size_t inputI1_ = 0, inputI2_ = 0;                  // InputVariable fields

    // Action to be taken, based on event type
    Action action_ = Action::NONE;                      // type of action to perform on the subordinate when replaying
    AddressInterval memoryLocation_;                    // memory locations affected by the action, if any
    uint64_t scalar_ = 0;                               // scalar value
    std::vector<uint8_t> bytes_;                        // byte data needed by action, if any
    SymbolicExpr::Ptr symbolic_;                        // symbolic expression for action, if any
    //     !!!!!!! DONT FORGET TO UPDATE ExecutionEvent::copy !!!!!!!!!

protected:
    ExecutionEvent();
public:
    ~ExecutionEvent();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Allocating constructor for unbound event. */
    static Ptr instance(rose_addr_t ip);

    /** Allocating constructor setting location.
     *
     *  Constructs an event at a particular location having no action. */
    static Ptr instance(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip);

    /** Allocating constructor for memory map events.
     *
     *  Creates an event at a particular location that will cause the specified addresses to be mapped. The protections
     *  are specified as a string containing zero or more of the letters "r", "w", or "x" in any order. If the test case
     *  and location are unspecified then the returned event has unbound.
     *
     * @{ */
    static Ptr instanceMapMemory(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                 const AddressInterval &addresses, const std::string &prot);
    static Ptr instanceMapMemory(rose_addr_t ip,
                                 const AddressInterval &addresses, const std::string &prot);
    /** @} */

    /** Allocating constructor for memory unmap events.
     *
     *  Creates an event at a particular location that will cause the specified memory addresses to be unmapped. If the
     *  test case and location are unspecified then the returned event is unbound.
     *
     * @{ */
    static Ptr instanceUnmapMemory(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                   const AddressInterval &addresses);
    static Ptr instanceUnmapMemory(rose_addr_t ip,
                                   const AddressInterval &addresses);
    /** @} */

    /** Allocating constructor for memory write events.
     *
     *  Creates an event at a particular location that will cause the specified memory addresses to be written with specified
     *  bytes.
     *
     *  If the test case and location are unspecified then the returned event is unbound.
     *
     * @{ */
    static Ptr instanceWriteMemory(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                   rose_addr_t va, const std::vector<uint8_t> &bytes);
    static Ptr instanceWriteMemory(rose_addr_t ip,
                                   rose_addr_t va, const std::vector<uint8_t> &bytes);

    template<typename Unsigned>
    static Ptr instanceWriteMemory(const TestCasePtr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                                   rose_addr_t va, Unsigned value) {
        Ptr retval = instanceWriteMemory(ip, va, value);
        retval->bind(tc, loc);
        return retval;
    }

    template<typename Unsigned>
    static Ptr instanceWriteMemory(rose_addr_t ip,
                                   rose_addr_t va, Unsigned value) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < sizeof value; ++i)
            bytes.push_back(BitOps::bits(value, i*8, i*8+7));
        return instanceWriteMemory(ip, va, bytes);
    }
    /** @} */

    /** Allocating constructor describing a memory hash.
     *
     *  Creates an event at a particular location that will hash the specified memory and compare it with the specified
     *  hash. The hash uses SHA256 since ROSE has a built-in version of this hasher.  If the test case and location are
     *  unspecified then the returned event is unbound.
     *
     * @{ */
    static Ptr instanceHashMemory(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                  const AddressInterval &addresses, const Combinatorics::Hasher::Digest&);
    static Ptr instanceHashMemory(rose_addr_t ip,
                                  const AddressInterval &addresses, const Combinatorics::Hasher::Digest&);
    /** @} */

    /** Allocating constructor describing one register.
     *
     *  Create an event at a particular location that will set the specified register to the specified value when replayed.
     *  If the test case and location are unspecified then the returned event is unbound.
     *
     *  The value should normally be concrete, but may be symbolic if this event follows a shared memory read event for
     *  the same instruction.
     *
     * @{ */
    static Ptr instanceWriteRegister(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                     RegisterDescriptor, uint64_t value);
    static Ptr instanceWriteRegister(rose_addr_t ip,
                                     RegisterDescriptor, uint64_t value);
    static Ptr instanceWriteRegister(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                     RegisterDescriptor, const SymbolicExpr::Ptr &value);
    static Ptr instanceWriteRegister(rose_addr_t ip,
                                     RegisterDescriptor, const SymbolicExpr::Ptr &value);
    /** @} */

    /** Allocating constructor describing all registers.
     *
     *  Create an even at a particular location that will initialize all registers to previously obtained values. If the
     *  test case and location are unspecified then the returned event is unbound.
     *
     * @{ */
    static Ptr instanceRestoreRegisters(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                        const Debugger::AllRegisters&);
    static Ptr instanceRestoreRegisters(rose_addr_t ip,
                                        const Debugger::AllRegisters&);
    /** @} */

    /** Allocating constructor for marking a system call.
     *
     *  A system call event is a marker for a system call, and the action depends on the system call. Usually the action
     *  only affects the simulated operating system and will be followed by zero or more other events to adjust the memory
     *  and registers in the subordinate process. If the test case and location are unspecified then the returned event is
     *  unbound.
     *
     * @{ */
    static Ptr instanceSyscall(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                               uint64_t functionNumber, const std::vector<uint64_t> &arguments);
    static Ptr instanceSyscall(rose_addr_t ip,
                               uint64_t functionNumber, const std::vector<uint64_t> &arguments);
    /** @} */

    /** Allocating constructor for marking a shared memory read.
     *
     *  A shared memory read event is a marker indicating that a read is occurring from a shared memory location. Usually the
     *  action only affects the simulated operating system and will be followed by zero or more additional events to take care
     *  of the side effects of the read operation. On RISC architectures, these side effects are typically just to move the
     *  read value into a register, but on CISC machines it might be more. If the test case and location are unspecified then
     *  the returned event is unbound.
     *
     * @{ */
    static Ptr instanceSharedMemoryRead(const TestCasePtr&, const ExecutionLocation&, rose_addr_t ip,
                                        rose_addr_t memoryAddress, size_t nBytes);
    static Ptr instanceSharedMemoryRead(rose_addr_t ip, rose_addr_t memoryAddress, size_t nBytes);
    /** @} */

    /** Make a copy of this event. */
    Ptr copy() const;

    /** Bind an execution event to a test case and location.
     *
     *  The @ref testCase and @ref location properties are set to the specified values. */
    void bind(const TestCasePtr&, const ExecutionLocation&);

    /** Property: Owning test case.
     *
     *  Each execution event belongs to exactly one test case.
     *
     * @{ */
    TestCasePtr testCase() const;
    void testCase(const TestCasePtr&);
    /** @} */

    /** Property: Database creation timestamp string.
     *
     *  Time stacmp string describing when this object was created in the database, initialized the first time the object is
     *  written to the database. If a value is assigned prior to writing to the database, then the assigned value is used
     *  instead. The value is typically specified in ISO-8601 format (except a space is used to separate the date and time for
     *  better readability, as in RFC 3339). This allows dates to be sorted chronologically as strings.
     *
     * @{ */
    const std::string& timestamp() const;
    void timestamp(const std::string&);
    /** @} */

    /** Property: Arbitrary name for debugging.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Event location.
     *
     * @{ */
    ExecutionLocation location() const;
    void location(const ExecutionLocation&);
    /** @} */

    /** Property: Instruction pointer.
     *
     *  This property is the value of the subordinate's instruction pointer register at the time the event occurs. This is
     *  mainly used as a cross check that we're at the correct event for this point of the execution.
     *
     * @{ */
    rose_addr_t instructionPointer() const;
    void instructionPointer(rose_addr_t);
    /** @} */

    /** Property: Test case input type.
     *
     * @{ */
    InputType inputType() const {
        return inputType_;
    }
    void inputType(InputType it) {
        inputType_ = it;
    }
    /** @} */

    /** Property: Input variable name.
     *
     *  Name of the symbolic variable associated with this event, if any.
     *
     * @{ */
    const SymbolicExpr::Ptr& inputVariable() const {
        return inputVariable_;
    }
    void inputVariable(const SymbolicExpr::Ptr &v) {
        inputVariable_ = v;
    }
    void inputVariable(const InstructionSemantics2::BaseSemantics::SValuePtr &variable);
    /** @} */

    /** Property: Input variable integer field one.
     *
     *  Stores input variable information depending on the @ref inputType.
     *
     * @{ */
    size_t inputI1() const {
        return inputI1_;
    }
    void inputI1(size_t i) {
        inputI1_ = i;
    }
    /** @} */

    /** Property: Input variable integer field two.
     *
     *  Stores input variable information depending on the @ref inputType.
     *
     * @{ */
    size_t inputI2() const {
        return inputI2_;
    }
    void inputI2(size_t i) {
        inputI2_ = i;
    }
    /** @} */

    /** Property: Type of action.
     *
     *  Each event has a corresponding action to be performend when the event is replayed.
     *
     * @{ */
    Action actionType() const;
    void actionType(Action);
    /** @} */

    /** Property: Memory location where acton occurs.
     *
     *  For actions that affect memory, this describes the memory addresses that are affected.
     *
     * @{ */
    AddressInterval memoryLocation() const;
    void memoryLocation(const AddressInterval&);
    /** @} */

    /** Property: Bytes for action.
     *
     *  This property holds any byte data necessary for the action.
     *
     * @{ */
    const std::vector<uint8_t>& bytes() const;
    std::vector<uint8_t>& bytes();
    void bytes(const std::vector<uint8_t>&);
    /** @} */

    /** Property: Bytes returned as a symbolic value.
     *
     *  The return value is a symbolic expression of type integer, whose width is eight times the number of bytes
     *  stored for the event. The bytes are assumed to be little endian.
     *
     *  If there are no bytes, then a null pointer is returned. */
    SymbolicExpr::Ptr bytesAsSymbolic() const;

    /** Property: Words for action.
     *
     *  This property interprets the @ref bytes as 64-bit little endian words.
     *
     * @{ */
    std::vector<uint64_t> words() const;
    void words(const std::vector<uint64_t>&);
    /** @} */

    /** Returns the bytes as a register value collection. */
    Debugger::AllRegisters allRegisters() const;

    /** Property: Scalar value.
     *
     *  Some events store a scalar value. The value is 64 bits but the interpretation depends on the event type.
     *
     * @{ */
    uint64_t scalar() const;
    void scalar(uint64_t);
    /** @} */

    /** Property: Symbolic value.
     *
     *  Some events store a symbolic expression. Usually these expressions are symbolic because they refer to an
     *  input variable(s) and will become concrete once concrete values are provided for that variable(s).
     *
     * @{ */
    SymbolicExpr::Ptr symbolic() const;
    void symbolic(const SymbolicExpr::Ptr&);
    /** @} */

    /** Print as YAML node. */
    void toYaml(std::ostream&, const DatabasePtr&, std::string prefix);

    /** Returns printable name of execution event for diagnostic output.
     *
     *  Returns a string suitable for printing to a terminal, containing the words "execution event", the database ID if
     *  appropriate, and the execution event name using C-style double-quoted string literal syntax if not empty.  The database
     *  ID is shown if a non-null database is specified and this executon event exists in that database. */
    std::string printableName(const DatabasePtr &db = DatabasePtr());
};

} // namespace
} // namespace
} // namespace

#endif
#endif
