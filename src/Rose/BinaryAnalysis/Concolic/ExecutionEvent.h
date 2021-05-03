#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
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
        NONE,                                           // Don't perform any action
        MAP_MEMORY,                                     // Add addresses to the memory map
        UNMAP_MEMORY,                                   // Remove addresses from the memory map
        WRITE_MEMORY,                                   // Change values in the memory map
        HASH_MEMORY,                                    // Verify that a memory region hashes to a particular value
        RESTORE_REGISTERS,                              // Set all register values
    };

    /** Location of event.
     *
     *  An event location consists of a primary value and a secondary value, both of which are unsigned integers. Depending on
     *  the architecture, the primary value might be the number of instructions executed (i.e, the length of the current
     *  execution path). The secondary value is usually just a sequence number for ordering events that all occur at the same
     *  primary value. */
    struct Location {
        uint64_t primary;                               /**< Primary location value. */
        uint64_t secondary;                             /**< Secondary location value. */

        Location()
            : primary(0), secondary(0) {}
        Location(uint64_t primary, uint64_t secondary)
            : primary(primary), secondary(secondary) {}
    };

private:
    std::string timestamp_;                             // time of creation, needed by the database
    TestCasePtr testCase_;                              // each event belongs to a particular test case

    // Event identification
    Location location_;                                 // location event occurs
    size_t instructionPointer_ = 0;                     // value of instruction pointer when event occurs

    // Action to be taken, based on event type
    Action action_ = Action::NONE;                      // type of action to perform on the subordinate when replaying
    AddressInterval memoryLocation_;                    // memory locations affected by the action, if any
    std::vector<uint8_t> bytes_;                        // byte data needed by action, if any

protected:
    ExecutionEvent();
public:
    ~ExecutionEvent();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Allocating constructor setting location.
     *
     *  Constructs an event at a particular location having no action. */
    static Ptr instance(const TestCasePtr&, const Location &location, rose_addr_t ip);

    /** Allocating constructor for memory map events.
     *
     *  Creates an event at a particular location that will cause the specified addresses to be mapped. The protections
     *  are specified as a string containing zero or more of the letters "r", "w", or "x" in any order. */
    static Ptr instanceMapMemory(const TestCasePtr&, const Location &location, rose_addr_t ip,
                                 const AddressInterval &addresses, const std::string &prot);

    /** Allocating constructor for memory unmap events.
     *
     *  Creates an event at a particular location that will cause the specified memory addresses to be unmapped. */
    static Ptr instanceUnmapMemory(const TestCasePtr&, const Location &location, rose_addr_t ip,
                                   const AddressInterval &addresses);

    /** Allocating constructor for memory write events.
     *
     *  Creates an event at a particular location that will cause the specified memory addresses to be written with specified
     *  bytes. */
    static Ptr instanceWriteMemory(const TestCasePtr&, const Location &location, rose_addr_t ip,
                                   const AddressInterval &addresses, const std::vector<uint8_t> &bytes);

    /** Allocating constructor describing a memory hash.
     *
     *  Creates an event at a particular location that will hash the specified memory and compare it with the specified
     *  hash. The hash uses SHA256 since ROSE has a built-in version of this hasher. */
    static Ptr instanceHashMemory(const TestCasePtr&, const Location &location, rose_addr_t ip,
                                  const AddressInterval &addresses, const Combinatorics::Hasher::Digest&);

    /** Allocating constructor describing all registers.
     *
     *  Create an even at a particular location tht will initialize all registers to previously obtained values. */
    static Ptr instanceRestoreRegisters(const TestCasePtr&, const Location &location, rose_addr_t ip,
                                        const Debugger::AllRegisters&);

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

    /** Property: Event location.
     *
     * @{ */
    Location location() const;
    void location(const Location&);
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

    /** Returns the bytes as a register value collection. */
    Debugger::AllRegisters allRegisters() const;

    /** Print as YAML node. */
    void toYaml(std::ostream&, const DatabasePtr&, std::string prefix);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
