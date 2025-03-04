#ifndef ROSE_BinaryAnalysis_Concolic_SharedMemory_H
#define ROSE_BinaryAnalysis_Concolic_SharedMemory_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/Yaml.h>

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

    /** Constructor when a new shared memory read event is encountered. */
    SharedMemoryContext(const ArchitecturePtr&, const Emulation::RiscOperatorsPtr&,
                        const ExecutionEventPtr &sharedMemoryEvent);

    /** Constructor for a shared memory write. */
    SharedMemoryContext(const ArchitecturePtr&, const Emulation::RiscOperatorsPtr&, Address accessingInstructionVa,
                        Address accessedVa, const SymbolicExpression::Ptr &value);

    virtual ~SharedMemoryContext();

    /*------------------------------------------------------------------------------------------------------------
     * Inputs to the callback
     *------------------------------------------------------------------------------------------------------------*/

    /** Phase of execution.
     *
     *  During the @c REPLAY phase, the callback's @ref SharedMemoryCallback::playback "playback" method is called,
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
    Address ip = 0;

    /** Addresses being accessed. */
    AddressInterval accessedVas;

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
    SymbolicExpression::Ptr valueRead;

    /** Value being written.
     *
     *  During a write operation, this is the value being written to memory. */
    SymbolicExpression::Ptr valueWritten;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks to handle shared memory operations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for shared memory callbacks. */
class SharedMemoryCallback: public Sawyer::SharedObject {
public:
    /** Reference counting pointer. */
    using Ptr = SharedMemoryCallbackPtr;

private:
    AddressInterval registrationVas_;                   // where in memory this callback was initially registered
    std::string name_;                                  // name of the callback in registrations and debugging

public:
    virtual ~SharedMemoryCallback();
protected:
    SharedMemoryCallback(const std::string &name);      // for factories
    SharedMemoryCallback(const AddressInterval&, const std::string &name);

public:
    /** Register a callback factory. */
    static void registerFactory(const Ptr &factory);

    /** Remove a registered factory.
     *
     *  The last occurrence of the specified factory is removed from the list of registered factories. This function returns
     *  true if a factory was removed, and false if no registered factories match. */
    static bool deregisterFactory(const Ptr &factory);

    /** List of all registered factories.
     *
     *  The returned list contains the registered factories in the order they were registered, which is the reverse order of
     *  how they're searched. */
    static std::vector<Ptr> registeredFactories();

    /** Instantiate a suitable object from the registered factories.
     *
     *  Scans the @ref registeredFactories list in the reverse order looking for a factory whose @ref matchFactory predicate
     *  returns true. The first factory whose predicate returns true is used to create and return a new object by invoking the
     *  factory's @c instanceFromFactory constructor. */
    static Ptr forge(const AddressInterval &where, const Yaml::Node &config);

    /** Predicate for matching a factory. */
    virtual bool matchFactory(const Yaml::Node &config) const;

    /** Virtual constructor for factories.
     *
     *  When invoked on a factory for which @ref matchFactory returns true, the factory will instantiate a new object with the
     *  same dynamic type as the factory. */
    virtual Ptr instanceFromFactory(const AddressInterval &where, const Yaml::Node &config) const;

    /** Returns true if this object is a factory. */
    bool isFactory() const;

public:
    /** Property: Name of callback.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Describes where the shared memory is registered.
     *
     *  When registering a shared memory callback with an @ref Architecture object, if this data member is empty then
     *  it will be initialized with the memory location at which the callback is being registered.  If you need to
     *  register a callback at more than one location, and the callbacks use this data member, then you should create
     *  separate callbacks for each registration.
     *
     * @{ */
    const AddressInterval& registrationVas() const;
    void registrationVas(const AddressInterval&);
    /** @} */

    /** Prints callback name and memory information. */
    void hello(const SharedMemoryContext&) const;

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

    /** Set the return value for a shared memory read.
     *
     *  If the specified return value is a constant, then the shared memory read will not be a variable input for a test case (equivalent
     *  to calling @ref notAnInput).
     *
     * @{ */
    void returns(SharedMemoryContext&, const SymbolicExpression::Ptr&) const;
    void returns(SharedMemoryContext&, const InstructionSemantics::BaseSemantics::SValuePtr&) const;
    /** @} */

    /** Input variable for future test cases.
     *
     *  Returns the input variable used to adjust future test cases. This is probably the variable you want to constrain in
     *  various ways according to the semantics of the shared memory.  Returns null if this read is not being treated as adjustable
     *  input in future test cases (see @ref notAnInput). */
    SymbolicExpressionPtr inputVariable(const SharedMemoryContext&) const;


    virtual bool operator()(bool handled, SharedMemoryContext&) final;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
