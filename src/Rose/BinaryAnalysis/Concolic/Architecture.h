#ifndef ROSE_BinaryAnalysis_Concolic_Architecture_H
#define ROSE_BinaryAnalysis_Concolic_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionLocation.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/Yaml.h>

#include <Sawyer/BitVector.h>

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Base class for architecture-specific operations. */
class Architecture: public Sawyer::SharedObject, public Sawyer::SharedFromThis<Architecture> {
public:
    /** Reference counting pointer. */
    using Ptr = ArchitecturePtr;

    /** Information about system calls. */
    using SystemCallMap = Sawyer::Container::Map<int /*syscall*/, SyscallCallbacks>;

    /** Information about shared memory. */
    using SharedMemoryMap = Sawyer::Container::IntervalMap<AddressInterval, SharedMemoryCallbacks>;

private:
    std::string name_;                                  // architecture name for factory matching
    DatabasePtr db_;
    TestCaseId testCaseId_;
    TestCasePtr testCase_;
    Partitioner2::PartitionerConstPtr partitioner_;
    Debugger::BasePtr debugger_;
    ExecutionLocation currentLocation_;                 // incremented when the instruction begins execution
    SystemCallMap systemCalls_;                         // callbacks for syscalls
    SharedMemoryMap sharedMemory_;                      // callbacks for shared memory
    InputVariablesPtr inputVariables_;                  // info about variables for events and inputs
    Sawyer::Optional<Address> scratchVa_;               // scratch page for internal use in subordinate address space

protected:
    // See "instance" methods in subclasses
    explicit Architecture(const std::string&);          // for factories
    Architecture(const DatabasePtr&, TestCaseId);       // for non-factories
public:
    virtual ~Architecture();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Factory
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Register an architecture as a factory.
     *
     *  The specified architecture is added to the end of a list of architecture prototypical objects. When a new architecture
     *  is needed, this list is scanned in reverse order until one of the @c matchFactory predicates for the prototypical
     *  object returns true, at which time a new copy of that prototypical object is created by passing the lookup arguments to
     *  its virtual @c instanceFromFactory constructor.
     *
     *  Thread safety: This method is thread safe. */
    static void registerFactory(const Ptr &factory);

    /** Remove an architecture factory from the registry.
     *
     *  The last occurrence of the specified factory is removed from the list of registered factories. This function returns
     *  true if a factory was removed, and false if no registered factories match.
     *
     *  Thread safety: This method is thread safe. */
    static bool deregisterFactory(const Ptr &factory);

    /** List of all registered factories.
     *
     *  The returned list contains the registered factories in the order they were registered, which is the reverse order
     *  of how they're searched.
     *
     *  Thread safety: This method is thread safe. */
    static std::vector<Ptr> registeredFactories();

    /** Creates a suitable architecture according to configuration information.
     *
     *  Scans the @ref registeredFactories list in the reverse order looking for a factory whose @ref matchFactory predicate
     *  (which accepts all but the first two arguments of this function) returns true. The first factory whose predicate
     *  returns true is used to create and return a new architecture object by invoking the factory's virtual @c
     *  instanceFromFactory constructor with the first two arguments of this function.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    static Ptr forge(const DatabasePtr&, TestCaseId, const Yaml::Node &config);
    static Ptr forge(const DatabasePtr&, const TestCasePtr&, const Yaml::Node &config);
    /** @} */

    /** Predicate for matching an architecture factory. */
    virtual bool matchFactory(const Yaml::Node &config) const = 0;

    /** Virtual constructor for factories.
     *
     *  This creates a new object by calling the class method @c instance for the class of which @c this is a type. All
     *  arguments are passed to @c instance. */
    virtual Ptr instanceFromFactory(const DatabasePtr&, TestCaseId, const Yaml::Node &config) const = 0;

    /** Returns true if this object is a factory.
     *
     *  Factories are created by the @c factory class methods rather than the usual @c instance class methods. A factory
     *  object should only be used to create other (non-factory) objects by registering it as a factory and eventually
     *  calling (directly or indirectly) its @ref instanceFromFactory object method. */
    bool isFactory() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Name.
     *
     *  The name of the architecture originally comes from the architecture factory, but can be changed on a per object
     *  basis.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Database.
     *
     *  Returns the database used as backing store for parts of this object. This property is read-only, set by the
     *  constructor. It is always non-null except in factories. */
    DatabasePtr database() const;

    /** Property: Test case ID.
     *
     *  Returns the test case ID within the database. This property is read-only, set by the constructor. The return value is
     *  always a valid ID except in factories. */
    TestCaseId testCaseId() const;

    /** Property: Test case.
     *
     *  Returns the test case being executed. This property is read-only, set by the constructor. The return value is always
     *  non-null except in factories. */
    TestCasePtr testCase() const;

    /** Property: Partitioner.
     *
     *  This holds information about the disassembly of the specimen, such as functions, basic blocks, and instructions. */
    Partitioner2::PartitionerConstPtr partitioner() const;
    void partitioner(const Partitioner2::PartitionerConstPtr&);

    /** Property: Debugger.
     *
     *  The debugger represents the concrete state of the specimen.
     *
     * @{ */
    Debugger::BasePtr debugger() const;
    void debugger(const Debugger::BasePtr&);
    /** @} */

    /** Property: Scratch page address.
     *
     *  Address of an optional scratch page in the concrete memory map. A scratch page is sometimes needed by the concrete
     *  executor in order to execute special code needed by the executor that isn't part of the normal process's image.
     *
     * @{ */
    Sawyer::Optional<Address> scratchVa() const;
    void scratchVa(const Sawyer::Optional<Address>&);
    /** @} */

    /** Property: Current execution location.
     *
     *  The execution location has three parts: a primary and a secondary, and whether it occurs before or after the
     *  corresponding instruction. The primary is the execution path length, and the secondary is a serial number that starts
     *  at zero for each primary value. Execution locations correspond to the locations stored in execution events, therefore
     *  when an event is created in a parent test case and copied to a child test case and then the child test case's events
     *  are replayed, the execution locations as the child is replayed must match the execution locations that existed in the
     *  parent when the execution events were created.
     *
     *  See also, @ref nextInstructionLocation and @ref nextEventLocation.
     *
     * @{ */
    ExecutionLocation currentLocation() const;
    void currentLocation(const ExecutionLocation&);
    /** @} */

    /** Property: Information about system calls.
     *
     *  This is a map indexed by system call number (e.g., SYS_getpid). The values of the map contain two types of information:
     *
     *  @li Information about how a system call should behave. For instance, SYS_getpid should return the same value each time
     *  it's called.
     *
     *  @li Information to make the declaraed behavior possible. For instance, the concrete and symbolic values returned the
     *  first time SYS_getpid was called so that we can make it return these same values in the future. */
    const SystemCallMap& systemCalls() const;

    /** Property: Information about shared memory.
     *
     *  This is a map indexed by concrete address. The values of the contain two types of information:
     *
     *  @li Information about how the shared memory region should behave. For instance, reads from shared memory that's
     *  attached to a timer would probably return monotonically increasing values.
     *
     *  @li Information to make the declared behavior possible. For instance, the concrete and symbolic values returned
     *  last time the timer's memory was read so that the new return value can be constrained to be greater than or equal
     *  to the previously returned value.
     *
     *  Each concrete address can have only one @ref SharedMemoryMap object, although each object can have a list of callbacks. */
    const SharedMemoryMap& sharedMemory() const;

    /** Properties: Input variables.
     *
     *  Keeps track of input variables and the bindings between variables (input or not) and their values as supplied by the
     *  execution events that have been encountered.
     *
     * @{ */
    InputVariablesPtr inputVariables() const;
    void inputVariables(const InputVariablesPtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions that can be called before execution starts.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Partition the specimen. */
    virtual Partitioner2::PartitionerPtr partition(const Partitioner2::EnginePtr&, const std::string &specimenName) = 0;

    /** Configures system call behavior.
     *
     *  This function declares how system calls are handled and is called from the @c instance methods (construction). */
    virtual void configureSystemCalls() = 0;

    /** Configures shared memory behavior.
     *
     *  This function declares how shared memory regions are handled and is called from the @c instance methods
     *  (constructors). The base class is responsible for installing shared memory callbacks that are described by
     *  the configuration passed in as an argument. The node should be a sequence of maps, one map per memory handler. */
    virtual void configureSharedMemory(const Yaml::Node &config);

    /** Add a shared memory callback for a range of addresses.
     *
     *  A callback normally knows the range of addresses for which it is responsible. That same range is used when registering the
     *  callback unless a specific range is specified. Specifying a range does not modify the @ref
     *  SharedMemoryCallback::registrationVas range that's inside the callback.  This can be useful when the same callback needs to
     *  be registered at multiple addresses.
     *
     * @{ */
    void sharedMemory(const SharedMemoryCallbackPtr&);
    void sharedMemory(const SharedMemoryCallbackPtr&, const AddressInterval&);
    /** @} */

    /** Add a callback for a system call number. */
    void systemCalls(size_t syscallId, const SyscallCallbackPtr&);

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
    virtual bool isTerminated();

    /** Build the symbolic instruction executor. */
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    makeDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions that create execution events. These query the concrete state but do not modify it.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create events that would map and initialize all the memory.
     *
     *  Reads all memory from the active test case (see constructor) and creates events that would map these memory segments
     *  and initialize them. The new events have no location or test case and are not yet written to a database. */
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() = 0;
    std::vector<ExecutionEventPtr> createMemoryRestoreEvents(const MemoryMapPtr&);

    /** Create events that check memory hashes.
     *
     *  This function hashes each memory segment and creates an event that when replayed will verify that the memory has
     *  the same hash. The returned events have no location or test case and are not yet written to a database. */
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() = 0;

    /** Create events that adjust memory mapping.
     *
     *  This is called after memory is inserted into or erased from the process memory map, such as in response to a brk or
     *  mmap2 system call.  It compares the process' current memory mapping with the provided mapping and adjusts the current
     *  mapping to match the concrete map by inserting and/or erasing certain regions. */
    virtual std::vector<ExecutionEventPtr> createMemoryAdjustEvents(const MemoryMapPtr&, Address insnVa) = 0;

    /** Create events that would restore register values.
     *
     *  This function reads all registers and creates events that when replayed would restore the registers to their saved
     *  values. */
    virtual std::vector<ExecutionEventPtr> createRegisterRestoreEvents();

    /** Saves a list of events.
     *
     *  Each event's test case is set to this object's test case, and each event has a new @ref ExecutionLocation assigned. Then
     *  the events are all written to the database. */
    void saveEvents(const std::vector<ExecutionEventPtr>&, When);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level functions controlling concrete execution.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Replay all events.
     *
     *  This is normally called immediately after @ref load. It processes all the events recorded in the database for this
     *  test case, causing the concrete execution to return to the state it was in after the last event.  Returns the number
     *  of events processed. */
    size_t playAllEvents(const Partitioner2::PartitionerConstPtr&);

    /** Replay an execution event.
     *
     *  Performs the action associated with the specified execution event, and returns true if the event was handled. If the
     *  implementation was unable to handle the event, then the return value is false.
     *
     *  This might adjust memory, registers, or do some other thing that may or may not affect the execution state. */
    virtual bool playEvent(const ExecutionEventPtr&);

    /** Run to the specified event.
     *
     *  While the current instruction is less than the specified event location, execute the instruction. */
    virtual void runToEvent(const ExecutionEventPtr&, const Partitioner2::PartitionerConstPtr&);

    /** Read memory bytes as an unsigned integer.
     *
     *  The number of bytes should be between one and eight, inclusive. */
    uint64_t readMemoryUnsigned(Address va, size_t nBytes);

    /** Read C-style NUL-terminated string from subordinate.
     *
     *  Reads up to @p maxBytes bytes or until an ASCII NUL character is read, concatenates all the characters (except the NUL)
     *  into a C++ string and returns it. The @p maxBytes includes the NUL terminator although the NUL terminator is not
     *  returned as part of the string. */
    virtual std::string readCString(Address va, size_t maxBytes = UNLIMITED);

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
    virtual Address ip();
    virtual void ip(Address);
    /** @} */

    /** Map a memory region.
     *
     *  For architectures that support it, memory is mapped at the specified addresses with the specified permissions. The
     *  @p permissions argument are the bits defined in the @ref MemoryMap class. */
    virtual void mapMemory(const AddressInterval &where, unsigned permissions);

    /** Unmap a memory region.
     *
     *  For architectures that support it, the specified region of memory is unmapped. */
    virtual void unmapMemory(const AddressInterval &where);

    /** Write bytes to memory.
     *
     *  Returns the number of bytes written, which might be fewer than the number requested if there is some kind of error. */
    virtual size_t writeMemory(Address startVa, const std::vector<uint8_t> &bytes);

    /** Read memory.
     *
     *  Reads the specified number of bytes from memory beginning at the specified address. Returns the number of bytes
     *  actually read, which might be fewer than the number requested if there is some kind of error. */
    virtual std::vector<uint8_t> readMemory(Address startVa, size_t nBytes);

    /** Write a value to a register.
     *
     *  Writes the specified value to the specified register. For values whose most significant set bit is index 63 or less,
     *  the value can be specified as a @c uint64_t. Other values will need to be specified as a bit vector.
     *
     * @{ */
    virtual void writeRegister(RegisterDescriptor, uint64_t value);
    virtual void writeRegister(RegisterDescriptor, const Sawyer::Container::BitVector&);
    /** @} */

    /** Read a value from a register. */
    virtual Sawyer::Container::BitVector readRegister(RegisterDescriptor);

    /** Make sure the executable has the same instruction in those bytes.
     *
     *  Looks at the concrete executable to see if it has the specified instruction at the instruction's address, and
     *  shows an error (optional) and throws an exception if not. */
    virtual void checkInstruction(SgAsmInstruction*);

    /** Execute an instruction concretely.
     *
     *  Executes the instruction concretely. For system calls, this only enters the system call. For other instructions
     *  it executes the current instruction concretely. */
    virtual void advanceExecution(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Execute current or specified instruction.
     *
     *  Executes the instruction and increments the length of the execution path.
     *
     * @{ */
    virtual void executeInstruction(const Partitioner2::PartitionerConstPtr&);
    virtual void executeInstruction(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, SgAsmInstruction*);
    /** @} */

    /** Increment the primary part of the current location.
     *
     *  The @c primary field of the @ref currentLocation property is incremented, the @c secondary field is set to zero, and
     *  the @c when field is set to @c AFTER. Returns the new location. */
    const ExecutionLocation& nextInstructionLocation();

    /** Increment the secondary part of the current location.
     *
     *  Increments the serial number for the @ref currentLocation property and returns the new location with its @c when
     *  property set to either @c PRE or @c POST.
     *
     * @{ */
    const ExecutionLocation& nextEventLocation(When);
    /** @} */

    /** Returns similar events.
     *
     *  Returns events that are for the same instruction as the specified event, but occur after it. The events are returned
     *  in the order they occur. */
    std::vector<ExecutionEventPtr> getRelatedEvents(const ExecutionEventPtr&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions related to symbolic states.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create initial input variables.
     *
     *  Reads the initial concrete state in order to create symbolic variables for initial input values such as the program
     *  arguments, number of program arguments, environment variables, auxilliary vector, etc. The new variables are added
     *  to the @ref inputVariables property.
     *
     *  Any interedependencies or other constraints on input variables should be added to the supplied SMT solver. */
    virtual void createInputVariables(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr&) = 0;

    /** Restore initial input variables.
     *
     *  This function is called when instantiating a test case that was created from a parent test case. It should read the
     *  database and recreate input variables that had been present in the parent test case, such as initial program arguments,
     *  environment variables, system call side effects, etc.
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
    virtual void restoreInputVariables(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                                       const SmtSolver::Ptr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions related to operating system emulation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Called when a system call occurs.
     *
     *  This function is called after a system call instruction has been executed symbolically and the system call has been
     *  entered concretely. */
    virtual void systemCall(const Partitioner2::PartitionerConstPtr&,
                            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) {}

    /** Called immediately when shared memory is accessed.
     *
     *  This function is called as soon as shared memory is accessed, right during the middle of an instruction from within the
     *  RiscOperators::readMemory operation.
     *
     *  For memory reads, it should either perform the operation and return the result, or return null in which case the caller
     *  will do the usual operation.
     *
     *  For memory writes, it should either perform the operation and return true, or return false in which case the caller
     *  will do the usual operation.
     *
     * @{ */
    virtual std::pair<ExecutionEventPtr, SymbolicExpressionPtr>
    sharedMemoryRead(const SharedMemoryCallbacks&, const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                     Address memVa, size_t nBytes);
    virtual bool
    sharedMemoryWrite(const SharedMemoryCallbacks&, const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                      Address memVa, const InstructionSemantics::BaseSemantics::SValuePtr&);
    /** @} */

    /** Run after-instruction shared memory callbacks.
     *
     *  Runs the @ref SharedMemoryCallback::handlePostSharedMemory "handlePostSharedMemory" callbacks for shared memory. This
     *  happens after the instruction that accessed shared memory has finished being emulated. */
    virtual void runSharedMemoryPostCallbacks(const ExecutionEventPtr &sharedMemoryEvent, const Emulation::RiscOperatorsPtr&);

    /** Fix up events related to shared memory.
     *
     *  This runs after the instruction accessing shared memory has been emulated, and after all shared memory callbacks for that
     *  instruction have returned. Its primary purpose is to figure out what value was read from memory even though that value
     *  might not appear directly anywhere in the concrete state. Consider an instruction like:
     *
     * @code
     *     r0 := u32_t [address] + 1
     * @endcode
     *
     *  Since this executes as a single instruction, there's no opportunity for us to query the conrete machine to get obtain
     *  the value that was read from memory, and we can't re-read that memory address because shared memory doesn't follow normal
     *  memory semantics (reading the address might have side effects of which we aren't aware).
     *
     *  This instruction will have generated at least two events:
     *
     *  @li An pre-instruction event that says that a shared memory read is about to occur, and a variable that represents the
     *  value read from memory. Call this variable "vr" for "value read".
     *
     *  @li A post-instruction register write event to fix up the register with the correct value. This event will have the
     *  @ref ExecutionEvent::expression @c vr+1. It will have no @ref ExecutionEvent::value because the event was created in the
     *  middle of emulating the instruction and a concrete value would not have been available at that time.
     *
     *  Now that the instruciton emulation is completed, we can ask the concrete state for the value of the register, a
     *  concrete value, and we assign this to the register read's @ref ExecutionEvent::value property. If we set the register
     *  write expression equal to this value and solve, we will get the value read from memory and we can populate the shared
     *  memory execution event with this value. */
    void fixupSharedMemoryEvents(const ExecutionEventPtr &sharedMemoryEvent, const Emulation::RiscOperatorsPtr&);

    /** Print information about shared memory events. */
    void printSharedMemoryEvents(const ExecutionEventPtr &sharedMemoryEvent, const Emulation::RiscOperatorsPtr&);

    /** Called after an instruction accesses shared memory. */
    virtual void sharedMemoryAccessPost(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
