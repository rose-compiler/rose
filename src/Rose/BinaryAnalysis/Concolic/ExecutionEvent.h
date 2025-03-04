#ifndef ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#define ROSE_BinaryAnalysis_Concolic_ExecutionEvent_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionLocation.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Combinatorics.h>                              // rose

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Execution event.
 *
 *  Execution events mark points during execution when something special must happen. Some examples:
 *
 *  @li When emulating an instruction concolically, the result of the instruction might depend on external factors that need to
 *  be preserved across runs. For instance, the @c getpid system call on Linux should return the same value for every variation
 *  of the test cases. Our goal is that the behavior of a test case should depend only on those things that we're treating as inputs, and
 *  perhaps @c getpid is not an input that we're trying to vary.
 *
 *  @li When creating a new test case from an existing test case, some input variables need to be assigned new values. The
 *  execution events indicate the points at which these values get written into the concrete state (and subsequently propagated
 *  by executing instructions concretely). For instance, if we indeed want to treat @c getpid as an input because perhaps the
 *  specimen does something special for certain PIDs, then the events specifie what value should be returned by each call.
 *
 *  @li When starting a test case, there's a "replay" phase where the instructions are replayed concretely to get the process
 *  into the correct state for concolic testing to start. The execution events are replayed as the execution path is
 *  followed. For instance, if there are five calls to @c getpid before we reach the starting point for concolic testing, then
 *  there will be five execution events to adjust the return value after each call.
 *
 *  @li Events can also be used to store information needed to initialize models for the operating system, peripherally
 *  attached devices, etc.
 *
 *  Most execution events have an associated action as stored in the event's @ref action property. Typical actions are: write
 *  some value to a certain register or memory, map or unmap regions of memory, initialize a set of registers or memory
 *  locations all at once, measure or validate memory and register contents for debugging purposes, indicate that a system call
 *  or shared memory operation is about to occur.
 *
 *  Every execution event has a location at which it exists in the execution path. Locations are represented by the @ref
 *  ExecutionLocation class, but in essence they consist of a triplet: the instruction number in the execution path to which
 *  they apply, an indication of whether the event is positioned before or after its instruction, and a serial number to order
 *  events that would otherwise be at the same location. In addition, each event stores the address of the instruction with
 *  which it's associated so that it can be checked at runtime (this is only a partial validation since the same instruction
 *  address can appear multiple times on an execution path).
 *
 *  Events that are used to fix up the concrete state, such as events that write a value to registers or memory either for
 *  reproducibility or for input variables, have three relevant properties: an optional @ref variable, a @ref value, and an
 *  optional symbolic @ref expression. When an event contains a variable and a value, the variable is bound to that value when
 *  the event is processed. When a concrete result is needed in order to update the concrete state, it is computed by evaluating
 *  the @ref expression in the presence of the @ref variable @ref value bindings for the current event and all previous events
 *  on the execution path. As an optimization, if the @ref expression property is empty, then the @ref value is used directly.
 *
 *  Events that have a @ref variable property can mark the variable as either being an input variable or not. An input variable
 *  is one that can be modified when creating new test cases. For instance, if we want to test what happens when we vary main's
 *  "argc" argument, we can represent that with a @c WRITE_MEMORY event that has a @ref variable marked as being an
 *  input. Input variables have additional properties to describe what type of input they are: argc, argv, environment,
 *  etc. Events are constructed without being input variables, and the input variable properties are adjusted after
 *  construction. */
class ExecutionEvent: public Sawyer::SharedObject, public Sawyer::SharedFromThis<ExecutionEvent> {
public:
    /** Reference-counting pointer to @ref ExecutionEvent. */
    using Ptr = ExecutionEventPtr;

    /** Side effects of events. */
    enum class Action {
        /** The event has no specific associated action. */
        NONE,

        /** Causes memory to be mapped into the address space.
         *
         *  The new addresses are initialized to zero. This action has a @ref memoryLocation and @ref permissions. */
        BULK_MEMORY_MAP,

        /** Causes memory to be unmapped from the address space.
         *
         *  This action has only a @ref memoryLocation. */
        BULK_MEMORY_UNMAP,

        /** Initialize a large area of memory.
         *
         *  This action's data are the @ref memoryLocation and the @ref bytes to write into that memory. See also,
         *  @c MEMORY_WRITE, which can be associated with an input variable. */
        BULK_MEMORY_WRITE,

        /** Hash and/or check a large area of memory.
         *
         *  The data for this action is the @ref memoryLocation and the @ref bytes for the hash of the memory's actual or
         *  expected contents. */
        BULK_MEMORY_HASH,

        /** Write a value to a memory location.
         *
         *  The data for this action is the @ref memoryLocation and the @ref value, @ref variable, and @ref expression used
         *  to intialize that location. */
        MEMORY_WRITE,

        /** Restore all registers with previously saved values.
         *
         *  The data for this action are the @ref bytes to write into the register set. The order and size of those bytes is
         *  architecture dependent. */
        BULK_REGISTER_WRITE,

        /** Write a value to a single register.
         *
         *  The data for this action is the @ref registerDescriptor, and the @ref value, @ref variable, and @ref expression used
         *  to initalize the register. */
        REGISTER_WRITE,

        /** Mark an instruction as being a system call.
         *
         *  This action indicates that the associated instruction is a system call. The data for this action is an integer to
         *  identify the system call function (@ref syscallFunction) and the potential or actual arguments for the function
         *  (@ref syscallArguments). These actions don't typically modify the concrete state of the executable, but rather are
         *  used to update the state for the simulated operating system. Additional events may be present to handle side
         *  effects of the system call, such as its return value (probably a @c REGISTER_WRITE event), changes to memory (@c
         *  MEMORY_WRITE), or others. */
        OS_SYSCALL,

        /** Mark an instruciton as accessing shared memory.
         *
         *  This action indicates that the associated instruction accesses shared memory. Shared memory does not follow normal
         *  memory semantics, it might be read-only or write-only, and values read or written might be computed by the
         *  instruction, making it somewhat difficult to intercept the access and replace it with desired values. Therefore,
         *  this event creates a @ref variable and @ref value binding for the memory access subsequent events will adjust
         *  side effects based on this binding.
         *
         *  For instance, if 0x1234 is an address in a shared memory region, then an instruction like x86 "xor eax, u32
         *  [0x1234]" reads four bytes of possibly read-only shared memory, exclusive-ORs those bytes with the contents in the
         *  eax register, and writes the result back into the eax register. When the instruction is single-stepped concretely,
         *  there's no point at which we can intercept the read to inject a new value. The best we can do is fix up eax after
         *  the fact, but we have to do it in terms of the input variable that describes the memory contents. Thus, this event
         *  will contain the binding between the input variable and the desired memory value, and a subsequent @c
         *  REGISTER_WRITE event will have an @ref expression that's a function of the input variable. */
        OS_SHARED_MEMORY
    };

private:
    // These are the data members that get read/written to the database.  The API properties defined for the various event
    // types are not necessarily a 1:1 mapping to these data members.

    // Event identification data members
    std::string timestamp_;                             // time of creation, needed by the database
    TestCasePtr testCase_;                              // each event belongs to a particular test case
    std::string name_;                                  // optional name for debugging
    ExecutionLocation location_;                        // location event occurs
    Address ip_ = 0;                                    // address of instruction associated with event

    // These data members map to properties, but not necessarily 1:1. It's done this way for efficiency in the database.
    Action action_ = Action::NONE;                      // type of action
    AddressInterval memoryVas_;                         // affected memory
    unsigned u_ = 0;                                    // permission bits, register, or syscall function number
    std::vector<uint8_t> bytes_;                        // byte data, hash digest, or syscall arguments
    SymbolicExpressionPtr variable_, value_, expression_; // for computing concrete result from input
    InputType inputType_ = InputType::NONE;             // what kind of input is variable_?
    size_t idx1_ = INVALID_INDEX;                       // indices for the input type, such as argv[i][j]
    size_t idx2_ = INVALID_INDEX;

protected:
    ExecutionEvent();
public:
    ~ExecutionEvent();

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Event constructors. These have camelCase names identical to the action.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Make a copy of this event. */
    Ptr copy() const;

    /** Allocating constructor for an empty execution event. */
    static Ptr instance();

    /** Allocating constructor for events with no action. */
    static Ptr noAction(const TestCasePtr&, const ExecutionLocation&, Address ip);


    /** Allocating constructor for @c BULK_MEMORY_MAP events. */
    static Ptr bulkMemoryMap(const TestCasePtr&, const ExecutionLocation&, Address ip,
                             const AddressInterval &where, unsigned permissions);

    /** Allocating constructor for @c BULK_MEMORY_UNMAP events. */
    static Ptr bulkMemoryUnmap(const TestCasePtr&, const ExecutionLocation&, Address ip,
                               const AddressInterval &where);

    /** Allocating constructor for @c BULK_MEMORY_WRITE events.
     *
     *  The number of bytes supplied must match the size of the memory region. */
    static Ptr bulkMemoryWrite(const TestCasePtr&, const ExecutionLocation&, Address ip,
                               const AddressInterval &where, const std::vector<uint8_t> &bytes);

    /** Allocating constructor for @c BULK_MEMORY_HASH events.
     *
     *  Hashing uses SHA256 and the supplied digest must be exactly 256 bits, or 32 bytes. */
    static Ptr bulkMemoryHash(const TestCasePtr&, const ExecutionLocation&, Address ip,
                              const AddressInterval &where, const Combinatorics::Hasher::Digest&);

    /** Allocating constructor for @c MEMORY_WRITE events. */
    static Ptr memoryWrite(const TestCasePtr&, const ExecutionLocation&, Address ip,
                           const AddressInterval &where, const SymbolicExpressionPtr &variable,
                           const SymbolicExpressionPtr &value, const SymbolicExpressionPtr &expression);

    /** Allocating constructor for @c BULK_REGISTER_WRITE events. */
    static Ptr bulkRegisterWrite(const TestCasePtr&, const ExecutionLocation&, Address ip,
                                 const Sawyer::Container::BitVector&);

    /** Allocating constructor for @c REGISTER_WRITE events. */
    static Ptr registerWrite(const TestCasePtr&, const ExecutionLocation&, Address ip,
                             RegisterDescriptor where, const SymbolicExpressionPtr &variable,
                             const SymbolicExpressionPtr &value, const SymbolicExpressionPtr &expression);

    /** Allocating constructor for @c OS_SYSCALL events. */
    static Ptr osSyscall(const TestCasePtr&, const ExecutionLocation&, Address ip,
                         unsigned function, const std::vector<uint64_t> &arguments);

    /** Allocating constructor for @c OS_SHARED_MEMORY events. */
    static Ptr osSharedMemory(const TestCasePtr&, const ExecutionLocation&, Address ip,
                              const AddressInterval &where, const SymbolicExpressionPtr &variable,
                              const SymbolicExpressionPtr &value, const SymbolicExpressionPtr &expression);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties that are used by all event types.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: Owning test case.
     *
     *  Each execution event belongs to exactly one test case.
     *
     *  Valid for all event types.
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
     *  Valid for all event types.
     *
     * @{ */
    const std::string& timestamp() const;
    void timestamp(const std::string&);
    /** @} */

    /** Property: Arbitrary name for debugging.
     *
     *  This name will appear in diagnostic messages. The accessor will return the stored name if non-empty, or else the
     *  comment or name of the associated @ref variable if present, or the empty string.
     *
     *  Valid for all event types.
     *
     * @{ */
    std::string name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Event location.
     *
     *  Describes where this event occurs within the execution path.
     *
     *  Valid for all event types.
     *
     * @{ */
    ExecutionLocation location() const;
    void location(const ExecutionLocation&);
    /** @} */

    /** Property: Instruction pointer.
     *
     *  The address of the instruction associated with this event. Note that the same instruction may appear multiple
     *  times on an execution path, so this is used only as a safety check. The event's actual location in the execution
     *  path is stored in the @ref location property.
     *
     *  Valid for all event types.
     *
     * @{ */
    Address instructionPointer() const;
    void instructionPointer(Address);
    /** @} */

    /** Property: Type of action.
     *
     *  Each event has a corresponding action to be performend when the event is replayed.
     *
     *  Valid for all even types.
     *
     * @{ */
    Action action() const;
    void action(Action);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties specific to certain types of events.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: Memory location where acton occurs.
     *
     *  For actions that affect memory, this describes the memory addresses that are affected.
     *
     *  Valid for types @c BULK_MEMORY_MAP, @c BULK_MEMORY_UNMAP, @c BULK_MEMORY_WRITE, @c BULK_MEMORY_HASH, @c MEMORY_WRITE,
     *  and @c OS_SHARED_MEMORY. Other types do not allow this property to be set, and always return an empty interval.
     *
     * @{ */
    AddressInterval memoryLocation() const;
    void memoryLocation(const AddressInterval&);
    /** @} */

    /** Property: Register affected by action.
     *
     *  Returns the register that's affected by the event action.
     *
     *  Valid for type @c REGISTER_WRITE. Other types do not allow this property to be set, and always return a default
     *  constructed, invalid register descriptor.
     *
     * @{ */
    RegisterDescriptor registerDescriptor() const;
    void registerDescriptor(RegisterDescriptor);
    /** @} */

    /** Property: Permissions for memory mapping.
     *
     *  This property holds the bits defined in @ref Rose::BinaryAnalysis::MemoryMap for describing memory access permissions.
     *
     *  Valid for type @c BULK_MEMORY_MAP. Other types do not allow this property to be set, and always return zero (no
     *  permissions granted).
     *
     * @{ */
    unsigned permissions() const;
    void permissions(unsigned);
    /** @} */

    /** Property: Data bytes for bulk operations.
     *
     *  This property stores the data to be written to a memory region or a register set.
     *
     *  Valid for types @c BULK_MEMORY_WRITE and @c BULK_REGISTER_WRITE. Other types do not allow this property to be set, and
     *  return an empty vector.
     *
     * @{ */
    const std::vector<uint8_t>& bytes() const;
    void bytes(const std::vector<uint8_t>&);
    /** @} */

    /** Property: Set of register values.
     *
     *  Valid for type @c BULK_REGISTER_WRITE. Other types do not permit this property to be set or retrieved.
     *
     * @{ */
    Sawyer::Container::BitVector registerValues() const;
    void registerValues(const Sawyer::Container::BitVector&);
    /** @} */

    /** Property: Hash value for checksumming.
     *
     *  This property stores the hash for actions that do hashing.
     *
     *  Valid for type @c BULK_MEMORY_HASH. Other types do not permit this property to be set, and will always return an
     *  empty digest.
     *
     * @{ */
    const Combinatorics::Hasher::Digest& hash() const;
    void hash(const Combinatorics::Hasher::Digest&);
    /** @} */

    /** Property: Variable to be bound.
     *
     *  The optional variable to be bound to the @ref value of this event during and after processing the event. During
     *  event playback, if a @ref variable is present then a @ref value must also be present.
     *
     *  Valid for types @c MEMORY_WRITE, @c REGISTER_WRITE, and @c OS_SHARED_MEMORY. Other types do not permit this property to
     *  be set, and always return a null pointer.
     *
     * @{ */
    SymbolicExpressionPtr variable() const;
    void variable(const SymbolicExpressionPtr&);
    /** @} */

    /** Property: Value to be bound.
     *
     *  The optional value to be bound to the @ref variable for this event during and after processing the event. During
     *  event playback, it is permissible for an event to have a @ref value but no @ref variable or @ref expression, in
     *  which case the @ref value is used directly without performing any substitution.
     *
     *  Valid for types @c MEMORY_WRITE, @c REGISTER_WRITE, and @c OS_SHARED_MEMORY. Other types do not allow this property to
     *  be set, and always return a null pointer.
     *
     * @{ */
    SymbolicExpressionPtr value() const;
    void value(const SymbolicExpressionPtr&);
    /** @} */

    /** Property: Expression defining result value.
     *
     *  The expression is evaluated in light of all current variable bindings from this and previous events in order to obtain
     *  a result that will be used for the action. For instance, if the action is @c REGISTER_WRITE, then the expression is
     *  evaluated, substiting the values for all bound variables, in order to obtain a concrete value to write to the register
     *  in the concrete state. See @ref calculateResult.
     *
     *  If the expression is absent, then the @ref value is used directly.
     *
     *  Valid for types @c MEMORY_WRITE, @c REGISTER_WRITE, and @c OS_SHARED_MEMORY. Other types do not allow this property to
     *  be set, and always return a null pointer.
     *
     * @{ */
    SymbolicExpressionPtr expression() const;
    void expression(const SymbolicExpressionPtr&);
    /** @} */

    /** Property: System call function identifier.
     *
     *  This property identifies the function invoked by a system call.
     *
     *  Valid for type @c OS_SYSCALL. Other types do not allow this property to be set, and always return zero.
     *
     * @{ */
    unsigned syscallFunction() const;
    void syscallFunction(unsigned);
    /** @} */

    /** Property: System call arguments.
     *
     *  These are either the actual or potential concrete system call arguments. In the case of Linux executables, there are
     *  always six potential arguments and a particular system call will use between zero and six of the arguments starting at
     *  the beginning of the list.
     *
     *  Valid for type @c OS_SYSCALL. Other types do not allow this property to be set, and always return an empty vector.
     *
     * @{ */
    std::vector<uint64_t> syscallArguments() const;
    void syscallArguments(const std::vector<uint64_t>&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties related to test case input variables.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Input variable type and indices.
     *
     *  If a @ref variable is present then this property specifies the type of input. When setting the input type, additional
     *  index parameters sometimes needed. For instance, input variables for the bytes of main's "argv" argument have two
     *  indices: the argument number and the character within the argument. The indices are ignored for input types that don't
     *  need them.
     *
     *  Valid for event types for which the @ref variable property is valid. Other types do not allow this property to be set, and
     *  always return @c NONE for the input type and @ref INVALID_INDEX for the indices.
     *
     * @{ */
    InputType inputType() const;
    void inputType(InputType, size_t idx1, size_t idx2);
    std::pair<size_t, size_t> inputIndices() const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Actions valid for all events
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Input variable if present.
     *
     *  Returns the input variable if this event has a @ref variable property that is valid and not null, and the @ref
     *  inputType property is not @c NONE.
     *
     *  Valid for all event types. */
    SymbolicExpressionPtr inputVariable() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Actions defined for some event types.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /** Calculate the concrete result.
     *
     *  The concrete result of an event is formed by evaluating the @ref expression given variable bindings from this and all
     *  previous events along the execution path.  The bindings are formed from the @ref variable and @ref value properties of
     *  those events.  If the @ref expression property of this event is empty, then the @ref value property is returned
     *  instead.
     *
     *  Valid for those event types that have @ref variable, @ref value, and @ref expression properties. */
    SymbolicExpressionPtr calculateResult(const SymbolicExpression::ExprExprHashMap &bindings) const;

    /** Returns printable name of execution event for diagnostic output.
     *
     *  Returns a string suitable for printing to a terminal, containing the words "execution event", the database ID, the @ref
     *  location, and the @ref name.  The database ID is shown if a non-null database is specified and this executon event
     *  exists in that database.
     *
     *  Valid for all event types. */
    std::string printableName(const DatabasePtr &db = DatabasePtr());

    /** Print event in YAML text format.
     *
     *  The first line uses the specified prefix, and subsequent lines use a prefix of all spaces which is the same length
     *  as that specified.
     *
     *  Valid for all event types. */
    void toYaml(std::ostream&, const DatabasePtr&, std::string prefix);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Database operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class DatabaseAccess;

    // Recreate the table for storing execution events. Is only called from the database layer.
    static void recreateTable(Sawyer::Database::Connection);

    // Save this object to the database. Is only called from the database layer.
    void toDatabase(const DatabasePtr&, ExecutionEventId);

    // Restore this object from the database. Is only called from the database layer.
    void fromDatabase(const DatabasePtr&, ExecutionEventId);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
