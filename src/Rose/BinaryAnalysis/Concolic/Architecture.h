#ifndef ROSE_BinaryAnalysis_Concolic_Architecture_H
#define ROSE_BinaryAnalysis_Concolic_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <ByteOrder.h>

#include <Sawyer/BitVector.h>

#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for architecture-specific operations. */
class Architecture: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = ArchitecturePtr;

private:
    DatabasePtr db_;
    TestCaseId testCaseId_;
    TestCasePtr testCase_;
    ExecutionLocation curLocation_;

protected:
    // See "instance" methods in subclasses
    Architecture(const DatabasePtr&, TestCaseId);
public:
    virtual ~Architecture();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Database.
     *
     *  Returns the database used as backing store for parts of this object. This property is read-only, set by the
     *  constructor. It is always non-null. */
    DatabasePtr database() const;

    /** Property: Test case ID.
     *
     *  Returns the test case ID within the database. This property is read-only, set by the constructor. The return value is
     *  always a valid ID. */
    TestCaseId testCaseId() const;

    /** Property: Test case.
     *
     *  Returns the test case being executed. This property is read-only, set by the constructor. The return value is always
     *  non-null. */
    TestCasePtr testCase() const;

    /** Property: Current execution location.
     *
     *  The execution location has two parts: a primary and a secondary. The primary is the execution path length, and the
     *  secondary is a serial number that starts at zero for each primary value. Execution locations correspond to the
     *  locations stored in execution events, therefore when an event is created in a parent test case and copied to a child
     *  test case and then the child test case's events are replayed, the execution locations as the child is replayed must
     *  match the execution locations that existed in the parent when the execution events were created.
     *
     *  See also, @ref incrementPathLength and @ref nextLocation.
     *
     * @{ */
    ExecutionLocation currentLocation() const;
    void currentLocation(const ExecutionLocation&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions that can be called before execution starts.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Prepares to execute the specimen concretely.
     *
     *  This should be called before calling any other functions that query or modify the execution state, such as those that read
     *  or write memory and registers.
     *
     *  The @p directory argument is the name of an existing directory that may be used to create temporary files related to the test
     *  case.
     *
     *  For example, the implementation might create a Linux process from the executable specimen for this object's test case. */
    virtual void load(const boost::filesystem::path &tempDirectory) = 0;

    /** True if process has terminated or never loaded.
     *
     *  If a process is in the terminated state, then most of the functions that query or modify the execution state are no
     *  longer well defined and should not be called. */
    virtual bool isTerminated() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions that create execution events. These query the concrete state but do not modify it.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create events that would map and initialize all the memory.
     *
     *  Reads all memory from the active test case (see constructor) and creates events that would map these memory segments
     *  and initialize them. The new events have no location or test case and are not yet written to a database. */
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() = 0;

    /** Create events that check memory hashes.
     *
     *  This function hashes each memory segment and creates an event that when replayed will verify that the memory has
     *  the same hash. The returned events have no location or test case and are not yet written to a database. */
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() = 0;

    /** Create events that would restore register values.
     *
     *  This function reads all registers and creates events that when replayed would restore the registers to their saved
     *  values. */
    virtual std::vector<ExecutionEventPtr> createRegisterRestoreEvents() = 0;

    /** Saves a list of events.
     *
     *  Each event's test case is set to this object's test case, and the event locations set by calling @ref nextLocation for
     *  each event. The events are written to the database. */
    void saveEvents(const std::vector<ExecutionEventPtr>&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level functions controlling concrete execution.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Replay all events.
     *
     *  This is normally called immediately after @ref load. It processes all the events recorded in the database for this
     *  test case, causing the concrete execution to return to the state it was in after the last event.  Returns the number
     *  of events processed. */
    size_t playAllEvents();

    /** Replay an execution event.
     *
     *  Performs the action associated with the specified execution event, and returns true if the event was handled. If the
     *  implementation was unable to handle the event, then the return value is false.
     *
     *  This might adjust memory, registers, or do some other thing that may or may not affect the execution state. */
    virtual bool playEvent(const ExecutionEventPtr&);

    /** Run to the specified event.
     *
     *  Execution is advanced until it reaches the specified event. */
    virtual void runToEvent(const ExecutionEventPtr&);

    /** Read memory bytes as an unsigned integer.
     *
     *  The number of bytes should be between one and eight, inclusive. */
    uint64_t readMemoryUnsigned(rose_addr_t va, size_t nBytes);

    /** Read C-style NUL-terminated string from subordinate.
     *
     *  Reads up to @p maxBytes bytes or until an ASCII NUL character is read, concatenates all the characters (except the NUL)
     *  into a C++ string and returns it. The @p maxBytes includes the NUL terminator although the NUL terminator is not
     *  returned as part of the string. */
    virtual std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level functions controlling concrete execution.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Memory byte order.
     *
     *  The order that bytes are arranged in memory when storing or reading multi-byte values such as uint64_t. */
    virtual ByteOrder::Endianness memoryByteOrder() = 0;

    /** Current execution address.
     *
     *  @{ */
    virtual rose_addr_t ip() = 0;
    virtual void ip(rose_addr_t) = 0;
    /** @} */

    /** Map a memory region.
     *
     *  For architectures that support it, memory is mapped at the specified addresses with the specified permissions. The
     *  @p permissions argument are the bits defined in the @ref MemoryMap class. */
    virtual void mapMemory(const AddressInterval &where, unsigned permissions) {}

    /** Unmap a memory region.
     *
     *  For architectures that support it, the specified region of memory is unmapped. */
    virtual void unmapMemory(const AddressInterval &where) {}

    /** Write bytes to memory.
     *
     *  Returns the number of bytes written, which might be fewer than the number requested if there is some kind of error. */
    virtual size_t writeMemory(rose_addr_t startVa, const std::vector<uint8_t> &bytes) = 0;

    /** Read memory.
     *
     *  Reads the specified number of bytes from memory beginning at the specified address. Returns the number of bytes
     *  actually read, which might be fewer than the number requested if there is some kind of error. */
    virtual std::vector<uint8_t> readMemory(rose_addr_t startVa, size_t nBytes) = 0;

    /** Write a value to a register.
     *
     *  Writes the specified value to the specified register. For values whose most significant set bit is index 63 or less,
     *  the value can be specified as a @c uint64_t. Other values will need to be specified as a bit vector.
     *
     * @{ */
    virtual void writeRegister(RegisterDescriptor, uint64_t value) = 0;
    virtual void writeRegister(RegisterDescriptor, const Sawyer::Container::BitVector&) = 0;
    /** @} */

    /** Read a value from a register. */
    virtual Sawyer::Container::BitVector readRegister(RegisterDescriptor) = 0;

    /** Execute current or specified instruction.
     *
     *  Executes the instruction and increments the length of the execution path.
     *
     * @{ */
    virtual void executeInstruction() = 0;
    virtual void executeInstruction(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, SgAsmInstruction*) = 0;
    /** @} */

    /** Increment the primary part of the current location.
     *
     *  The primary field of the @ref currentLocation property is incremented, and the secondary field is set to zero. Returns
     *  the new location. */
    const ExecutionLocation& incrementPathLength();

    /** Increment the secondary part of the current location.
     *
     *  Increments the serial number for the @ref currentLocation property and returns the new location. */
    const ExecutionLocation& nextLocation();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions related to symbolic states.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create initial input variables.
     *
     *  Reads the initial concrete state in order to create symbolic variables for initial input values such as the program
     *  arguments, number of program arguments, environment variables, auxilliary vector, etc. The new variables are added
     *  to the @p inputVariables argument.
     *
     *  Any interedependencies or other constraints on input variables should be added to the supplied SMT solver. */
    virtual void createInputVariables(InputVariables &inputVariables /*in,out*/, const Partitioner2::Partitioner&,
                                      const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr&) = 0;

    /** Restore initial input variables.
     *
     *  This function is called when instantiating a test case that was created from a parent test case. It should read the
     *  database and recreate input variables that had been present in the parent test case, such as initial program arguments,
     *  environment variables, system call side effects, etc. The new variables are added to the @p inputVariables argument.
     *
     *  The following actions have already occured by time this function is called:
     *
     *  @li The concrete state has been initialized to appear as if the program has executed up to the point where the parent
     *  test case had created it. This involved the replaying of all execution events.
     *
     *  @li The symbolic state attached to the RiscOperators argument is initialized to be the same as it was when the parent
     *  test case created this test case.
     *
     *  @li The SMT solver's assertions have been initialized to be the same as when the parent test case created this test
     *  case. In particular, the solver contains the assertions for the current execution path in terms of input variables, as
     *  well as all the assertions for input variables from the parent test case. */
    virtual void restoreInputVariables(InputVariables &inputVariables /*in,out*/, const Partitioner2::Partitioner&,
                                       const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                       const SmtSolver::Ptr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions related to operating system emulation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Called when a system call occurs.
     *
     *  This function is called after a system call instruction has been executed symbolically and the system call has been
     *  entered concretely. */
    virtual void systemCall(const Partitioner2::Partitioner&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
