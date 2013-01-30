#ifndef Rose_BaseSemantics_H
#define Rose_BaseSemantics_H

#include "Registers.h"
#include "FormatRestorer.h"
#include "SMTSolver.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// Documented elsewhere
namespace BinaryAnalysis {

/** Binary instruction semantics.
 *
 *  Entities in this namespace deal with the semantics of machine instructions, and with the process of "executing" a machine
 *  instruction in a particular semantic domain.  Instruction "execution" is a very broad term and can refer to execution in
 *  the tranditional sense where each instruction modifies the machine state (registers and memory) in a particular domain
 *  (concrete, interval, sign, symbolic, user-defined).  But it can also refer to any kind of analysis that depends on
 *  semantics of individual machine instructions (def-use, taint, etc).  It can even refer to the transformation of machine
 *  instructions in ROSE internal representation to some other representation (e.g., ROSE RISC or LLVM) where the other
 *  representation is built by "executing" the instruction.
 *
 * @section IS1 Components of instruction semantics
 *
 *  ROSE's binary semantics framework has two major components: the dispatchers and the semantic domains. The instruction
 *  dispatcher "executes" a machine instruction by translating it into a sequence of RISC-like operations, and the semantics
 *  domain defines what the RISC-operators do (e.g., change a concrete machine state, produce an output listing of RISC
 *  operations, build an LLVM representation).
 *
 *  ROSE defines one @em dispatcher class per machine architecture. In this respect, the dispatcher is akin to the
 *  microcontroller for a CISC architecture, such as the x86 microcontroller within an x86 CPU. Users can subclass the
 *  dispatcher if they need to override how a machine instruction is translated into RISC operations. (Note: ROSE might move
 *  away from the huge "switch" statement implementation to a table-driven approach, in which case subclassing of the entire
 *  dispatcher will likely be replaced by either modifying the dispatch table or by subclassing individual machine instruction
 *  classes.)
 *
 *  The <em>semantic domain</em> is a loose term that refers to at least three parts taken as a whole: a value type, a machine
 *  state type, and the RISC operators.  Semantic domains have names like "concrete domain", "interval domain", "sign domain",
 *  "symbolic domain", etc.  The term is used loosely since one could have different implementations of, say, a "concrete
 *  domain" by using different combinations of dispatcher, state, and value type classes. For instance, one concrete domain
 *  might use the PartialSymbolicSemantics classes in a concrete way, while another might use custom classes tuned for higher
 *  performance.  ROSE defines a set of semantic domains--each defined by grouping its three components (value type, machine
 *  state type, and RISC operations) into a single name space or class.
 *
 *  The <em>values</em> of a semantic domain (a.k.a., "svalues") are defined by a class type for that domain.  For instance, a
 *  concrete domain's value type would likely hold bit vectors of varying sizes.  Instantiations of the value type are used for
 *  register contents, memory contents, memory addresses, and temporary values that exist during execution of a machine
 *  instruction. Every value has a width measured in bits.  For instance, an x86 architecture needs values that are 1, 5, 8,
 *  16, 32, and 64 bits wide (the 1-bit values are for Booleans in the EFLAGS register; the five-bit values are shift counts on
 *  a 32-bit architecutre; the 64-bit values are needed for integer multiply on a 32-bit architecture; this list is likely not
 *  exhaustive).  Various kinds of value type form a class hierarchy whose most basic type is BaseSemantics::SValue.
 *
 *  As instructions execute they use inputs and generate outputs, which are read from and written to a <em>machine
 *  state</em>. The machine state consists of registers and memory, each of which holds a value which is instantiated from the
 *  domain's value type.  Furthermore, memory addresses are also described by instances of the domain's value type (although
 *  internally, they can use a different type as long as a translation is provided to and from that type).  The names and
 *  inter-relationships of the architecture's registers are contained in a RegisterDictionary while the state itself contains
 *  the values stored in those registers.  The organization of registers and memory within the state is defined by the state.
 *  Various kinds of states form a class hierarchy whose most basic type is BaseSemantics::State.
 *
 *  The <em>RISC operators</em> class provides the implementations for the RISC operators.  An instance of the RISC operators
 *  class is provided as an argument to the dispatcher constructor in order to create a dispatcher with the specified RISC
 *  semantics.  It is possible (and normal) to create multiple versions of a dispatcher each with a different combination of
 *  value type, state, and RISC operators and each tuned for a specific kind of analysis.
 *
 *  @section IS3 Memory Management
 *
 *  Most of the instruction semantics components have abstract base classes. Instances of concrete subclasses thereof are
 *  passed around by pointers, and in order to simplify memory management issues, those objects are reference counted using
 *  Boost smart pointers.  For convenience, pointer typedefs are created for each class--their names are the same as the class
 *  but suffixed with "Ptr".  Users will almost exclusively work with pointers to the objects rather than objects themselves
 *  (the one exception is during object construction).
 *
 *  Implementations of an interface will frequently need to create a new instance of a class even though all they know is the
 *  name of the abstract base class.  Therefore, the base class defines at least one virtual constructor so that given an
 *  object (which is an instance of one of the subclasses), that virtual constructor can be invoked to create another object of
 *  the same dynamic type.  In fact, reference counted objects will define three versions of each constructor: a virtual
 *  constructor, a class-wide constructor, and the actual constructor.  The first two versions return a smart pointer while the
 *  third is the usual C++ constructor.  The C++ constructor should have protected access to help prevent users from
 *  inadvertently instantiating such an object on the stack, or using an object without reference counting it.
 *
 *  From a user's perspective, reference counted objects are constructed from a class known at compile time by calling one of
 *  the class-wide constructors whose name is usually instance():
 *
 *  @code
 *      // user code
 *      #include <IntervalSemantics.h>
 *      using namespace BinaryAnalysis::InstructionSemantics;
 *      BaseSemantics::SValuePtr value = IntervalSemantics::SValue::instance();
 *      // no need to ever delete the object that 'value' points to
 *  @endcode
 *
 *  When subclassing in order to override or agument a method from the base class, the method might need to create an instance
 *  of a class without knowing the name of the class--all it has is an existing object of that class.  The new object can be
 *  created by invoking one of the virtual constructors upon the existing object.  Copyable objects will also define a clone()
 *  virtual constructor that creates an exact copy of the object.  For example:
 *
 *  @code
 *      BaseSemantics::SValuePtr func1(const BaseSemantics::SValuePtr &operand, int addend) {
 *          BaseSemantics::SValuePtr retval = operand->create(operand->sum + addend);
 *          return retval;
 *      }
 *  @endcode
 *
 *  When writing a subclass, each constructor will have three versions as mentioned above:
 *
 *  @code
 *      typedef boost::shared_ptr<class MyThing> MyThingPtr;
 *      class MyThing: public OtherThing { // subclass of BaseSemantics::Thing
 *      private:
 *          double value;
 *      protected:
 *          // the usual C++ constructors
 *          MyThing(size_t width, double v): OtherThing(width), value(v) {}
 *      public:
 *          // the class-wide constructors, all named "instance"
 *          static MyThingPtr instance(size_t width, double v) {
 *              return MyThingPtr(new MyThing(width, v));
 *          }
 *          // the virtual constructors (no naming convention)
 *          virtual BaseSemantics::ThingPtr generate(size_t width, double v) const override {
 *              return instance(width, v);
 *          }
 *     };
 *  @endcode
 *
 *  @section IS4 Specialization
 *
 *  The instruction semantics architecture is designed to allow users to specialize nearly every part of it.  ROSE defines
 *  triplets (value type, state type, RISC operators) that are designed to work together to implement a particular semantic
 *  domain, but users are free to subclass any of those components to build customized semantic domains.  For example, the x86
 *  simulator (in "projects/simulator") subclasses the PartialSymbolicSemantics state in order to use memory mapped via ROSE's
 *  MemoryMap class, and its RISC operators in order to handle system calls (among other things).
 *
 *  @section IS5 Future work
 *
 *  <em>Table-driven dispatch.</em> The current dispatchers are implemented with a huge switch statement selecting for each
 *  possible machine instruction.  This design doesn't lend itself well to users being able to augment/override individual
 *  instructions, users adding new instructions, enabling large groups of instructions individually (e.g., SSE) to handle
 *  variations in machine architecture since each modification requires a subclass.  The plan is to replace the large "switch"
 *  statement with a dispatch table and replace each "case" with a functor specific to that machine instruction.  Users will be
 *  able to modify individual table entries, modify related groups of table entries, subclass functors for individual
 *  instructions, or define new functors.  The functors will likely be class templates that take arguments similar to the
 *  dispatcher's class template.
 *
 *  <em>Floating-point instructions.</em> Floating point registers are defined in the various RegisterDictionary objects but
 *  none of the semantic states actually define space for them, and we haven't defined any floating-point RISC operations for
 *  policies to implement.  As for existing machine instructions, the dispatchers will translate machine floating point
 *  instructions to RISC operations, and the specifics of those operations will be defined by the various semantic policies.
 *  For instance, the RISC operators for a concrete semantic domain might use the host machine's native IEEE floating point to
 *  emulate the target machine's floating-point operations.
 *
 *  @section IS6 Example
 *
 *  See actual source code for examples since this interface is an active area of ROSE development (as of Jan-2013). In order
 *  to use one of ROSE's predefined semantic domains you'll likely need to define some types and variables, something along
 *  these lines:
 *
 *  @code
 *   using namespace BinaryAnalysis::InstructionSemantics;
 *   BaseSemantics::SValuePtr protoval = SymbolicSemantics::SValuePtr::instance();
 *   BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateX86::instance(protoval);
 *   BaseSemantics::MemoryStatePtr memory = BaseSemantics::MemoryCellList::instance(protoval);
 *   BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
 *   BaseSemantics::RiscOperatorsPtr operators = SymbolicSemantics::RiscOperators::instance(state);
 *   BaseSemantics::DispatcherPtr semantics = DispatcherX86::instance(operators);
 *  @endcode
 *
 *  In order to analyze a sequence of instructions, one calls the dispatcher's processInstruction() method one instruction at a
 *  time.  The dispatcher breaks the instruction down into a sequence of RISC-like operations and invokes those operations in
 *  the chosen semantic domain.  The RISC operations produce domain-specific result values and/or update the machine state
 *  (registers, memory, etc).  Each RISC operator domain provides methods by which the user can inspect and/or modify the
 *  state.  In fact, in order to follow flow-of-control from one instruction to another, it is customary to read the x86 EIP
 *  (instruction pointer register) value to get the address for the next instruction fetch.
 *
 *  One can find actual uses of instruction semantics in ROSE by searching for DispatcherX86.  Also, the simulator project (in
 *  projects/simulator) has many examples how to use instruction semantics--in fact, the simulator defines its own concrete
 *  domain by subclassing PartialSymbolicSemantics in order to execute specimen programs.
 */
namespace InstructionSemantics {

/** Base classes for instruction semantics.  Basically, anything that is common to two or more instruction semantic
 *  domains will be factored out and placed in this name space. */
namespace BaseSemantics {

/** Smart pointer to an SValue object. SValue objects are reference counted through boost::shared_ptr smart pointers. The
 *  underlying object should never be explicitly deleted. */ 
typedef boost::shared_ptr<class SValue> SValuePtr;

/** Helper class for printing. Some semantic domains may need to pass some additional information to print methods on a
 *  per-call basis.  This base class provides something they can subclass to do that. A (optional) pointer to an instance of
 *  this class is passed to all semantic print() methods. */
class PrintHelper {
public:
    virtual ~PrintHelper() {}
};

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

/** Base class for exceptions thrown by instruction semantics. */
class Exception {
public:
    std::string mesg;
    SgAsmInstruction *insn;
    Exception(const std::string &mesg, SgAsmInstruction *insn): mesg(mesg), insn(insn) {}
    virtual ~Exception() {}
    virtual void print(std::ostream&) const;
};

/*******************************************************************************************************************************
 *                                      Semantic Values
 *******************************************************************************************************************************/

/** Base class for semantic values. Semantics value objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::SValue is an abstract class that defines the interface.  See the BinaryAnalysis::InstructionSemantics
 *  namespace for an overview of how the parts fit together.*/
class SValue {
protected:
    size_t width;                               /** Width of the value in bits. Typically (not always) a power of two. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
    explicit SValue(size_t nbits): width(nbits) {}

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Class-based constructors.  None are needed--this class is abstract.
    virtual ~SValue() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.  false_(), true_(), and undefined_() need underscores, so we do so consistently for all c'tors.

    /** Create a new undefined semantic value.  The new semantic value will have the same dynamic type as the value
     *  on which this virtual method is called.  This is the most common way that a new value is created. */
    virtual SValuePtr undefined_(size_t nbits) const = 0;

    /** Create a new concrete semantic value. The new value will represent the specified concrete value and have the same
     *  dynamic type as the value on which this virtual method is called. This is the most common way that a new constant is
     *  created. */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const = 0;

    /** Create a new, false Boolean value. The new semantic value will have the same dynamic type as the value on
     *  which this virtual method is called. This is how 1-bit flag register values (among others) are created. The base
     *  implementation uses number_() to construct a 1-bit value whose bit is clear. */
    virtual SValuePtr false_() const { return number_(1, 0); }

    /** Create a new, true Boolean value.  The new semantic value will have the same dynamic type as the value on
     *  which this virtual method is called. This is how 1-bit flag register values (among others) are created. The base
     *  implementation uses number_() to construct a 1-bit value whose bit is set. */
    virtual SValuePtr true_() const { return number_(1, 1); }

    /** Create a new value from an existing value, changing the width if @p new_width is non-zero. Increasing the width
     *  logically adds zero bits to the most significant side of the value; decreasing the width logically removes bits from the
     *  most significant side of the value. */
    virtual SValuePtr copy_(size_t new_width=0) const = 0;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...

    /** Determines if the value is a concrete number. Concrete numbers can be created with the number_(), true_(), or false_()
     *  virtual constructors, or by other means. */
    virtual bool is_number() const = 0;

    /** Return the concrete number for this value.  Only values for which is_number() returns true are able to return a
     *  concrete value by this method. */
    virtual uint64_t get_number() const = 0;

    /** Accessor for value width.
     * @{ */
    virtual size_t get_width() const { return width; }
    virtual void set_width(size_t nbits) { width = nbits; }
    /** @} */

    /** Returns true if two values could be equal. The SMT solver is optional for many subclasses. */
    virtual bool may_equal(const SValuePtr &other, SMTSolver *solver=NULL) const = 0;

    /** Returns true if two values must be equal.  The SMT solver is optional for many subclasses. */
    virtual bool must_equal(const SValuePtr &other, SMTSolver *solver=NULL) const = 0;

    /** Print a value to a stream. The value will normally occupy a single line and not contain leading space or line
     * termination. */
    virtual void print(std::ostream &output, PrintHelper *helper=NULL) const = 0;
};



/*******************************************************************************************************************************
 *                                      Register States
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** The set of all registers and their values. RegisterState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::RegisterState is an abstract class that defines the interface.  See the BinaryAnalysis::InstructionSemantics
 *  namespace for an overview of how the parts fit together.*/
class RegisterState {
protected:
    SValuePtr protoval;                         /**< Prototypical value for virtual constructors. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
    explicit RegisterState(const SValuePtr &protoval): protoval(protoval) {
        assert(protoval!=NULL);
    }

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Class-based constructors.  None are needed--this class is abstract.
    virtual ~RegisterState() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.

    /** Virtual constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be used only
     *  to create additional instances of the value via its virtual constructors.  The prototypical value is normally of the
     *  same type for all parts of a semantic analysis. */
    virtual RegisterStatePtr create(const SValuePtr &protoval) const = 0;

    /** Make a copy of this register state. */
    virtual RegisterStatePtr clone() const = 0;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** Set all registers to distinct undefined values. */
    virtual void clear() = 0;

    /** Set all registers to the zero. */
    virtual void zero() = 0;

    /** Read a value from a register. */
    virtual SValuePtr readRegister(const RegisterDescriptor &reg) = 0;

    /** Write a value to a register. */
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value) = 0;

    /** Print the register contents. This emits one line per register and contains the register name and its value.
     *  The @p ph argument is an optional PrintHelper that is simply passed as the second argument of the
     *  underlying print methods for the register values. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const = 0;
};

/** The set of all registers and their values for an x86 architecture. */
class RegisterStateX86: public RegisterState {
public:
    static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
    static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
    static const size_t n_flags = 32;           /**< Number of flag registers in this state. */

    SValuePtr ip;                               /**< Instruction pointer. */
    SValuePtr gpr[n_gprs];                      /**< General-purpose registers */
    SValuePtr segreg[n_segregs];                /**< Segmentation registers. */
    SValuePtr flag[n_flags];                    /**< Control/status flags (i.e., FLAG register). */

protected:
    explicit RegisterStateX86(const SValuePtr &protoval): RegisterState(protoval) {}

public:
    /** Constructor. The @p protoval argument must be a non-null pointer to a semantic value which will be used only to create
     *  additional instances of the value via its virtual constructors.  The prototypical value is normally of the same type
     *  for all parts of a semantic analysis: its state and operator classes. */
    static RegisterStatePtr instance(const SValuePtr &protoval) {
        return RegisterStatePtr(new RegisterStateX86(protoval));
    }

    virtual RegisterStatePtr create(const SValuePtr &protoval) const /*override*/;
    virtual RegisterStatePtr clone() const /*override*/;

    virtual void clear() /*override*/;
    virtual void zero() /* override*/;
    virtual SValuePtr readRegister(const RegisterDescriptor &reg) /*override*/;
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value) /*override*/;
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const /*override*/;
};


/*******************************************************************************************************************************
 *                                      Memory State
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Represents all memory in the state. MemoryState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::MemoryState is an abstract class that defines the interface.  See the BinaryAnalysis::InstructionSemantics
 *  namespace for an overview of how the parts fit together.*/
class MemoryState {
protected:
    SValuePtr protoval;                         /**< Prototypical value. */

    explicit MemoryState(const SValuePtr &protoval): protoval(protoval) {
        assert(protoval!=NULL);
    }

public:
    virtual ~MemoryState() {}

    /** Virtual constructor.  Constructs a new MemoryState object having the same dynamic type as this object. */
    virtual MemoryStatePtr create(const SValuePtr &protoval) const = 0;

    /** Virtual copy constructor. Creates a new MemoryState object which is a copy of this object. */
    virtual MemoryStatePtr clone() const = 0;

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** Clear memory. Removes all memory cells from this memory state. */
    virtual void clear() = 0;

    /** Read a value from memory. */
    virtual SValuePtr readMemory(const SValuePtr &addr) = 0;

    /** Write a value to memory. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value) = 0;

    /** Print a memory state to more than one line of output. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *helper=NULL) const = 0;
};


/******************************************************************************************************************
 *                                  Cell List Memory State
 ******************************************************************************************************************/

/** Represents one location in memory.  Each memory cell has an address and a value. */
class MemoryCell {
protected:
    SValuePtr address;                          /**< Address of memory cell. */
    SValuePtr value;                            /**< Value stored at that address. */

public:
    /** Constructor. Creates a new memory cell object with the specified address and value. */
    MemoryCell(const SValuePtr &address, const SValuePtr &value)
        : address(address), value(value) {
        assert(address!=NULL);
        assert(value!=NULL);
        assert(value->get_width()==8);
    }

    /** Accessor for the memory cell address.
     * @{ */
    virtual SValuePtr get_address() const { return address; }
    virtual void set_address(const SValuePtr &addr) {
        assert(addr!=NULL);
        address = addr;
    }
    /** @}*/

    /** Accessor for the value stored at a memory location.
     * @{ */
    virtual SValuePtr get_value() const { return value; }
    virtual void set_value(const SValuePtr &v) {
        assert(v!=NULL);
        value = v;
    }
    /** @}*/

    /** Print the memory cell on a single line. */
    virtual void print(std::ostream &o, PrintHelper *helper=NULL) const {
        o <<"addr=";
        address->print(o, helper);
        o <<" value=";
        value->print(o, helper);
    }
};


typedef boost::shared_ptr<class MemoryCellList> MemoryCellListPtr;

/** Simple list-based memory state. */
class MemoryCellList: public MemoryState {
protected:
    std::list<MemoryCell> cells;

    explicit MemoryCellList(const SValuePtr &protoval): MemoryState(protoval) {}

public:
    static MemoryCellListPtr instance(const SValuePtr &protoval) {
        return MemoryCellListPtr(new MemoryCellList(protoval));
    }

    virtual MemoryStatePtr create(const SValuePtr &protoval) const /*override*/ {
        return instance(protoval);
    }

    virtual MemoryStatePtr clone() const /*override*/ {
        return MemoryStatePtr(new MemoryCellList(*this));
    }

    virtual void clear() /*override*/ {
        cells.clear();
    }

    /** Read a value from memory. This implementation reads memory by scanning through the list of memory cells until it
     *  finds the most recent write to an address that is equal the the specified address.  Subclasses will almost certainly
     *  want to override this in order to support aliasing. */
    virtual SValuePtr readMemory(const SValuePtr &addr) /*override*/;

    /** Write a value to memory. This implementation just creates a new memory cell and inserts it into the front of the
     *  memory cell list. Subclasses will almost certainly want to override this behavior. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value) /*override*/;

    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *helper=NULL) const /*override*/;
};

/******************************************************************************************************************
 *                                  State
 ******************************************************************************************************************/

typedef boost::shared_ptr<class State> StatePtr;

/** Base class for semantics machine states.
 *
 *  Binary semantic analysis usually progresses one instruction at a time--one starts with an initial state and the act of
 *  processing an instruction modifies the state.  The State is the base class class for the semantic states of various
 *  instruction semantic policies.  It contains storage for all the machine registers and memory.
 *
 *  States must be copyable objects.  Many analyses keep a copy of the machine state for each instruction or each CFG
 *  vertex.
 *
 *  State objects are allocated on the heap and reference counted.  The BaseSemantics::State is an abstract class that defines
 *  the interface.  See the BinaryAnalysis::InstructionSemantics namespace for an overview of how the parts fit together.  */
class State {
protected:
    SValuePtr protoval;                         /**< Initial value used to create additional values as needed. */
    RegisterStatePtr registers;                 /**< All machine register values for this semantic state. */
    MemoryStatePtr  memory;                     /**< All memory for this semantic state. */

protected:
    // Normal constructors are protected because this class is reference counted. */
    State(const RegisterStatePtr &registers, const MemoryStatePtr &memory)
        : registers(registers), memory(memory) {
        assert(registers!=NULL);
        assert(memory!=NULL);
        protoval = registers->get_protoval();
        assert(protoval!=NULL);
    }

    /** States must be copyable objects.  Many analyses depend on being able to make a copy of the entire semantic state at
     *  each machine instruction, at each CFG vertex, etc. */
    State(const State &other) {
        protoval = other.protoval;
        registers = registers->clone();
        memory = memory->clone();
    }

public:
    virtual ~State() {}

    // deep-copy the registers and memory so users can think of the machine state as a single entity.
    State& operator=(const State &other) {
        protoval = other.protoval;
        registers = registers->clone();
        memory = memory->clone();
        return *this;
    }

    /** Constructor. */
    static StatePtr instance(const RegisterStatePtr &registers, const MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    /** Virtual constructor. */
    virtual StatePtr create(const RegisterStatePtr &registers, const MemoryStatePtr &memory) const {
        return instance(registers, memory);
    }

    /** Virtual copy constructor. */
    virtual StatePtr clone() const {
        return create(registers->clone(), memory->clone());
    }

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** Initialize state.  The register and memory states are cleared. */
    virtual void clear() {
        registers->clear();
        memory->clear();
    }

    /** Initialize all registers to zero. Memory is not affected. */
    virtual void zero_registers() {
        registers->zero();
    }

    /** Clear all memory locations.  This just empties the memory vector. */
    virtual void clear_memory() {
        memory->clear();
    }

    /** Read a value from a register. */
    virtual SValuePtr readRegister(const RegisterDescriptor &desc) {
        return registers->readRegister(desc);
    }

    /** Write a value to a register. */
    virtual void writeRegister(const RegisterDescriptor &desc, const SValuePtr &value) {
        registers->writeRegister(desc, value);
    }

    /** Read a value from memory. */
    virtual SValuePtr readMemory(const SValuePtr &addr) {
        return memory->readMemory(addr);
    }

    /** Write a value to memory. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value) {
        memory->writeMemory(addr, value);
    }

    /** Print the register contents. This emits one line per register and contains the register name and its value.  The @p ph
     *  argument is an optional PrintHelper that is simply passed as the second argument of the underlying print methods for
     *  the SValue. */
    virtual void print_registers(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
        registers->print(o, prefix, ph);
    }

    /** Print memory contents.  This simply calls the MemoryState::print method. The @p ph argument is an optional PrintHelper
     * that's passed as the second argument to the underlying print methods for the SValue. */
    virtual void print_memory(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
        memory->print(o, prefix, ph);
    }

    /** Print the state.  This emits a multi-line string containing the registers and all known memory locations.  The @p ph
     *  argument is an optional PrintHelper pointer that's simply passed as the second argument to the print methods for the
     *  SValue. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const {
        o <<prefix <<"registers:\n";
        print_registers(o, prefix+"    ", ph);
        o <<prefix <<"memory:\n";
        print_memory(o, prefix+"    ", ph);
    }
};

/******************************************************************************************************************
 *                                  RISC Operators
 ******************************************************************************************************************/

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Base class for most instruction semantics RISC operators.  This class is responsible for defining the semantics of the
 *  RISC-like operations invoked by the translation object (e.g., X86InstructionSemantics).  We omit the definitions for most
 *  of the RISC operations from the base class so that failure to implement them in a subclass is an error.
 *
 *  RiscOperator objects are allocated on the heap and reference counted.  The BaseSemantics::RiscOperator is an abstract class
 *  that defines the interface.  See the BinaryAnalysis::InstructionSemantics namespace for an overview of how the parts fit
 *  together. */
class RiscOperators {
protected:
    SValuePtr protoval;                         /**< Prototypical value used for its virtual constructors. */
    StatePtr state;                             /**< State upon which RISC operators operate. */
    SgAsmInstruction *cur_insn;                 /**< Current instruction, as set by latest startInstruction() call. */
    size_t ninsns;                              /**< Number of instructions processed. */

protected:
    explicit RiscOperators(const SValuePtr &protoval): protoval(protoval), cur_insn(NULL), ninsns(0) {
        assert(protoval!=NULL);
    }
    explicit RiscOperators(const StatePtr &state): state(state), cur_insn(NULL), ninsns(0) {
        assert(state!=NULL);
        protoval = state->get_protoval();
    }
    /** @} */

public:
    virtual ~RiscOperators() {}

    /** Virtual constructor.  The @p protoval is a prototypical semantic value that is used as a factory to create additional
     *  values as necessary via its virtual constructors.  The state upon which the RISC operations operate must be provided by
     *  a separate call to the set_state() method. */
    virtual RiscOperatorsPtr create(const SValuePtr &protoval) const = 0;

    /** Constructor.  The supplied @p state is that upon which the RISC operations operate and is also used to define the
     *  prototypical semantic value. Other states can be supplied by calling set_state(). The prototypical semantic value
     *  is used as a factory to create additional values as necessary via its virtual constructors. */
    virtual RiscOperatorsPtr create(const StatePtr &state) const = 0;

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** Access the state upon which the RISC operations operate. The state need not be set until the first instruction is
     *  executed (and even then, some RISC operations don't need any machine state (typically, only register and memory read
     *  and write operators need state).  Different state objects can be swapped in at pretty much any time.  Modifying the
     *  state has no effect on this object's prototypical value which was initialized by the constructor; new states should
     *  have a prototyipcal value of the same dynamic type.
     * @{ */
    StatePtr get_state() { return state; }
    void set_state(const StatePtr &s) { state = s; }
    /** @} */

    /** Print multi-line output for this object. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *helper=NULL) const {
        state->print(o, prefix, helper);
    }

    /** Returns the number of instructions processed. This counter is incremented at the beginning of each instruction. */
    virtual size_t get_ninsns() const {
        return ninsns;
    }

    /** Sets the number instructions processed. This is the same counter incremented at the beginning of each instruction and
     *  returned by get_ninsns(). */
    virtual void set_ninsns(size_t n) {
        ninsns = n;
    }

    /** Returns current instruction. Returns the null pointer if no instruction is being processed. */
    virtual SgAsmInstruction *get_insn() const {
        return cur_insn;
    }

    /** Called at the beginning of every instruction.  This method is invoked every time the translation object begins
     *  processing an instruction.  Some policies use this to update a pointer to the current instruction. */
    virtual void startInstruction(SgAsmInstruction *insn) {
        assert(insn!=NULL);
        cur_insn = insn;
        ++ninsns;
    };

    /** Called at the end of every instruction.  This method is invoked whenever the translation object ends processing for an
     *  instruction.  This is not called if there's an exception during processing. */
    virtual void finishInstruction(SgAsmInstruction *insn) {
        assert(insn);
        assert(cur_insn==insn);
        cur_insn = NULL;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Value Construction Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The trailing underscores are necessary for true_() and false_() on all machines and necessary for undefined_() on some
    // machines, so we just add one to the end of all the virtual constructors for consistency.

    /** Returns a true value. Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr true_() {
        return protoval->true_();
    }

    /** Returns a false value. Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr false_() {
        return protoval->false_();
    }

    /** Returns a new undefined value. Uses the prototypical value to virtually construct the new value. */
    virtual SValuePtr undefined_(size_t nbits) {
        return protoval->undefined_(nbits);
    }

    /** Returns a number of the specified bit width.  Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr number_(size_t nbits, uint64_t value) {
        return protoval->number_(nbits, value);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  x86-specific Operations (FIXME)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked to filter call targets.  This method is called whenever the translation object is about to invoke a function
     *  call.  The target address is passed as an argument and a (new) target should be returned. */
    virtual SValuePtr filterCallTarget(const SValuePtr &a) {
        return a;
    }

    /** Invoked to filter return targets.  This method is called whenever the translation object is about to return from a
     *  function call (such as for the x86 "RET" instruction).  The return address is passed as an argument and a (new) return
     *  address should be returned. */
    virtual SValuePtr filterReturnTarget(const SValuePtr &a) {
        return a;
    }

    /** Invoked to filter indirect jumps.  This method is called whenever the translation object is about to unconditionally
     *  jump to a new address (such as for the x86 "JMP" instruction).  The target address is passed as an argument and a (new)
     *  target address should be returned. */
    virtual SValuePtr filterIndirectJumpTarget(const SValuePtr &a) {
        return a;
    }

    /** Invoked for the x86 HLT instruction. */
    virtual void hlt() {}

    /** Invoked for the x86 CPUID instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual void cpuid() {}

    /** Invoked for the x86 RDTSC instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual SValuePtr rdtsc() { return undefined_(64); }

    /** Invoked for the x86 INT instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual void interrupt(uint8_t) {}

    /** Invoked for the x86 SYSENTER instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual void sysenter() {};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Boolean Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Computes bit-wise AND of two values. The operands must both have the same width; the result must be the same width as
     *  the operands. */
    virtual SValuePtr and_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Computes bit-wise OR of two values. The operands @p a and @p b must have the same width; the return value width will
     * be the same as @p a and @p b. */
    virtual SValuePtr or_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Computes bit-wise XOR of two values. The operands @p a and @p b must have the same width; the result will be the same
     *  width as @p a and @p b. */
    virtual SValuePtr xor_(const SValuePtr &a, const SValuePtr &b) = 0;

    /** One's complement. The result will be the same size as the operand. */
    virtual SValuePtr invert(const SValuePtr &a) = 0;

    /** Extracts bits from a value.  The specified bits from begin_bit (inclusive) through end_bit (exclusive) are copied into
     *  the low-order bits of the return value (other bits in the return value are cleared). The least significant bit is
     *  number zero. The begin_bit and end_bit values must be valid for the width of @p a. */
    virtual SValuePtr extract(const SValuePtr &a, size_t begin_bit, size_t end_bit) = 0;

    /** Concatenates the bits of two values.  The bits of @p a and @p b are concatenated so that the result has @p
     *  b in the high-order bits and @p a in the low order bits. The width of the return value is the sum of the widths of @p
     *  a and @p b. */
    virtual SValuePtr concat(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Returns position of least significant set bit; zero when no bits are set. The return value will have the same width as
     * the operand, although this can be safely truncated to the log-base-2 + 1 width. */
    virtual SValuePtr leastSignificantSetBit(const SValuePtr &a) = 0;

    /** Returns position of most significant set bit; zero when no bits are set. The return value will have the same width as
     * the operand, although this can be safely truncated to the log-base-2 + 1 width. */
    virtual SValuePtr mostSignificantSetBit(const SValuePtr &a) = 0;

    /** Rotate bits to the left. The return value will have the same width as operand @p a. */
    virtual SValuePtr rotateLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Rotate bits to the right. The return value will have the same width as operand @p a. */
    virtual SValuePtr rotateRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted left. The return value will have the same width as operand @p a. */
    virtual SValuePtr shiftLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right logically (no sign bit). The return value will have the same width as operand @p a. */
    virtual SValuePtr shiftRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right arithmetically (with sign bit). The return value will have the same width as operand @p a. */
    virtual SValuePtr shiftRightArithmetic(const SValuePtr &a, const SValuePtr &nbits) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Comparison Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Determines whether a value is equal to zero.  Returns true, false, or undefined (in the semantic domain) depending on
     *  whether argument is zero. */
    virtual SValuePtr equalToZero(const SValuePtr &a) = 0;

    /** If-then-else.  Returns operand @p a if @p cond is true, operand @p b if @p cond is false, or some other value if the
     *  condition is unknown. The @p condition must be one bit wide; the widths of @p a and @p b must be equal; the return
     *  value width will be the same as @p a and @p b. */
    virtual SValuePtr ite(const SValuePtr &cond, const SValuePtr &a, const SValuePtr &b) = 0;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Integer Arithmetic Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Extend (or shrink) operand @p a so it is @p nbits wide by adding or removing high-order bits. Added bits are always
     *  zeros. The result will be the specified @p new_width. */
    virtual SValuePtr unsignedExtend(const SValuePtr &a, size_t new_width) {
        return a->copy_(new_width);
    }

    /** Sign extends a value. The result will the the specified @p new_width, which must be at least as large as the original
     * width. */
    virtual SValuePtr signExtend(const SValuePtr &a, size_t new_width) = 0;

    /** Adds two integers of equal size.  The width of @p a and @p b must be equal; the return value will have the same width
     * as @p a and @p b. */
    virtual SValuePtr add(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Add two values of equal size and a carry bit.  Carry information is returned via carry_out argument.  The carry_out
     *  value is the tick marks that are written above the first addend when doing long arithmetic like a 2nd grader would do
     *  (of course, they'd probably be adding two base-10 numbers).  For instance, when adding 00110110 and 11100100:
     *
     *  \code
     *    '''..'..         <-- carry tick marks: '=carry .=no carry
     *     00110110
     *   + 11100100
     *   ----------
     *    100011010
     *  \endcode
     *
     *  The carry_out value is 11100100.
     *
     *  The width of @p a and @p b must be equal; @p c must have a width of one bit; the return value and @p carry_out will be
     *  the same width as @p a and @p b.  The @p carry_out value is allocated herein. */
    virtual SValuePtr addWithCarries(const SValuePtr &a, const SValuePtr &b, const SValuePtr &c,
                                        SValuePtr &carry_out/*output*/) = 0;

    /** Two's complement. The return value will have the same width as the operand. */
    virtual SValuePtr negate(const SValuePtr &a) = 0;

    /** Divides two signed values. The width of the result will be the same as the width of operand @p a. */
    virtual SValuePtr signedDivide(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Calculates modulo with signed values. The width of the result will be the same as the width of operand @p b. */
    virtual SValuePtr signedModulo(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Multiplies two signed values. The width of the result will be the sum of the widths of @p a and @p b. */
    virtual SValuePtr signedMultiply(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Divides two unsigned values. The width of the result is the same as the width of operand @p a. */
    virtual SValuePtr unsignedDivide(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Calculates modulo with unsigned values. The width of the result is the same as the width of operand @p b. */
    virtual SValuePtr unsignedModulo(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Multiply two unsigned values. The width of the result is the sum of the widths of @p a and @p b. */
    virtual SValuePtr unsignedMultiply(const SValuePtr &a, const SValuePtr &b) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  State Accessing Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Reads a value from a register. */
    virtual SValuePtr readRegister(const RegisterDescriptor &reg) {
        assert(state!=NULL);
        return state->readRegister(reg);
    }

    /** Writes a value to a register. */
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &a) {
        assert(state!=NULL);
        state->writeRegister(reg, a);
    }

    /** Reads a value from memory. The @p cond argument is a Boolean value that indicates whether this is a true read
     *  operation. */
    virtual SValuePtr readMemory(X86SegmentRegister sg, const SValuePtr &addr, const SValuePtr &cond, size_t nbits) = 0;


    /** Writes a value to memory. The @p cond argument is a Boolean value that indicates whether this is a true write
     * operation. */
    virtual void writeMemory(X86SegmentRegister sg, const SValuePtr &addr, const SValuePtr &data, const SValuePtr &cond) = 0;
};


/*******************************************************************************************************************************
 *                                      Instruction Dispatcher
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

/** Functor that knows how to dispatch a single kind of instruction. */
class InsnProcessor {
public:
    virtual ~InsnProcessor() {}
    virtual void process(const DispatcherPtr &dispatcher, SgAsmInstruction *insn) = 0;
};
    
/** Dispatches instructions through the RISC layer.  The dispatcher is the instruction semantics entity that translates a
 *  high-level architecture-dependent instruction into a sequence of RISC operators whose interface is defined by ROSE. These
 *  classes are the key in ROSE's ability to connect a variety of instruction set architectures to a variety of semantic
 *  domains.
 *
 *  Dispatcher objects are allocated on the heap and reference counted.  The BaseSemantics::Dispatcher is an abstract class
 *  that defines the interface.  See the BinaryAnalysis::InstructionSemantics namespace for an overview of how the parts fit
 *  together. */
class Dispatcher: public boost::enable_shared_from_this<Dispatcher> {
protected:
    const RegisterDictionary *regdict;          /**< See set_register_dictionary(). */
    RiscOperatorsPtr operators;

    explicit Dispatcher(const RiscOperatorsPtr &ops): operators(ops) {
        regdict = RegisterDictionary::dictionary_i386();
        assert(operators!=NULL);
    }

    // Dispatchers keep a table of all the kinds of instructions they can handle.  The lookup key is typically some sort of
    // instruction identifier, such as from SgAsmx86Instruction::get_kind(), and comes from the iproc_key() virtual method.
    typedef std::vector<InsnProcessor*> InsnProcessors;
    InsnProcessors iproc_table;

public:
    virtual ~Dispatcher() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Virtual constructor. */
    virtual DispatcherPtr create(const RiscOperatorsPtr &ops) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods to process instructions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Process a single instruction. */
    virtual void processInstruction(SgAsmInstruction *insn);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Instruction processor table operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Lookup the processor for an instruction.  Looks up the functor that has been registered to process the given
     *  instruction. Returns the null pointer if the instruction cannot be processed. Instruction processor objects are
     *  managed by the caller; the instruction itself is only used for the duration of this call. */
    virtual InsnProcessor *iproc_lookup(SgAsmInstruction *insn);

    /** Replace an instruction processor with another.  The processor for the specified instruction is replaced with the
     *  specified processor, which may be the null pointer.  Instruction processor objects are managed by the caller; the
     *  instruction itself is only used for the duration of this call. */
    virtual void iproc_replace(SgAsmInstruction *insn, InsnProcessor *iproc);    

protected:
    /** Given an instruction, return the InsnProcessor key that can be used as an index into the iproc_table. */
    virtual int iproc_key(SgAsmInstruction*) const = 0;

    /** Set an iproc table entry to the specified value. */
    virtual void iproc_set(int key, InsnProcessor *iproc);

    /** Obtain an iproc table entry for the specified key. */
    virtual InsnProcessor *iproc_get(int key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods that defer the call to some member object
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Get a pointer to the RISC operators object. */
    virtual RiscOperatorsPtr get_operators() const { return operators; }

    /** Get a pointer to the state object. The state is stored in the RISC operators object, so this is just here for
     *  convenience. */
    virtual StatePtr get_state() const { return operators->get_state(); }

    /** Return the prototypical value.  The prototypical value comes from the RISC operators object. */
    virtual SValuePtr get_protoval() const { return operators->get_protoval(); }

    /** Returns the instruction that is being processed. The instruction comes from the get_insn() method of the RISC operators
     *  object. */
    virtual SgAsmInstruction *get_insn() const { return operators->get_insn(); }

    /** Return a new undefined semantic value. */
    virtual SValuePtr undefined_(size_t nbits) const { return operators->undefined_(nbits); }

    /** Return a semantic value representing a number. */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const { return operators->number_(nbits, number); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods related to registers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Access the register dictionary.  The register dictionary defines the set of registers over which the RISC operators may
     *  operate. This should be same registers (or superset) of registers whose values are stored in the machine state(s).
     *  This dictionary is used by the Dispatcher class to translate register names to register descriptors.  For instance, to
     *  read from the "eax" register, the dispatcher will look up "eax" in its register dictionary and then pass that
     *  descriptor to the readRegister() RISC operation.  Register descriptors are also stored in instructions when the
     *  instruction is disassembled, so the dispatcher should probably be using the same registers as the disassembler, or a
     *  superset thereof.
     *
     *  The register dictionary should not be changed after a dispatcher is instantiated because the dispatcher's constructor
     *  may query the dictionary and cache the resultant register descriptors.
     * @{ */
    virtual const RegisterDictionary *get_register_dictionary() {
        return regdict;
    }
    virtual void set_register_dictionary(const RegisterDictionary *regdict) {
        this->regdict = regdict;
    }
    /** @} */

    /** Lookup a register by name.  This dispatcher's register dictionary is consulted and the specified register is located by
     *  name.  If a bit width is specified (@p nbits) then it must match the size of register that was found.  If a valid
     *  register cannot be found then an exception is thrown. */
    virtual const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0);

};

/*******************************************************************************************************************************
 *                                      Printing
 *******************************************************************************************************************************/

std::ostream& operator<<(std::ostream&, const Exception&);
std::ostream& operator<<(std::ostream&, const SValue&);
std::ostream& operator<<(std::ostream&, const MemoryCell&);
std::ostream& operator<<(std::ostream&, const MemoryState&);
std::ostream& operator<<(std::ostream&, const RegisterState&);
std::ostream& operator<<(std::ostream&, const State&);
std::ostream& operator<<(std::ostream&, const RiscOperators&);

} /*namespace*/
} /*namespace*/
} /*namespace*/
#endif
