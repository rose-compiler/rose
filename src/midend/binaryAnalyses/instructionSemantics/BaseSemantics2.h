#ifndef Rose_BaseSemantics2_H  
#define Rose_BaseSemantics2_H

#include "Diagnostics.h"
#include "Registers.h"
#include "FormatRestorer.h"
#include "SMTSolver.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <sawyer/Assert.h>
#include <sawyer/IntervalMap.h>
#include <sawyer/Map.h>
#include <sawyer/Optional.h>

namespace rose {
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
 *  ROSE defines one <em>dispatcher</em> class per machine architecture. In this respect, the dispatcher is akin to the
 *  microcontroller for a CISC architecture, such as the x86 microcontroller within an x86 CPU.  The base class for all
 *  dispatchers is BaseSemantics::Dispatcher.
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
 *  concrete domain's value type would likely hold bit vectors of varying sizes.  Instances of the value type are used for
 *  register contents, memory contents, memory addresses, and temporary values that exist during execution of a machine
 *  instruction. Every value has a width measured in bits.  For instance, an x86 architecture needs values that are 1, 5, 8,
 *  16, 32, and 64 bits wide (the 1-bit values are for Booleans in the EFLAGS register; the five-bit values are shift counts on
 *  a 32-bit architecutre; the 64-bit values are needed for integer multiply on a 32-bit architecture; this list is likely not
 *  exhaustive).  Various kinds of value type form a class hierarchy whose root is BaseSemantics::SValue.
 *
 *  As instructions execute they use inputs and generate outputs, which are read from and written to a <em>machine
 *  state</em>. The machine state consists of registers and memory, each of which holds a value which is instantiated from the
 *  domain's value type.  Furthermore, memory addresses are also described by instances of the domain's value type (although
 *  internally, they can use a different type as long as a translation is provided to and from that type).  The names and
 *  inter-relationships of the architecture's registers are contained in a RegisterDictionary while the state itself contains
 *  the values stored in those registers.  The organization of registers and memory within the state is defined by the state.
 *  Various kinds of states form a class hierarchy whose root is BaseSemantics::State.
 *
 *  The <em>RISC operators</em> class provides the implementations for the RISC operators.  Those operators are documented in
 *  the BaseSemantics::RiscOperators class, which is the root of a class hierarchy.  Most of the RISC operators are pure
 *  virtual.
 *
 *  In order to use binary instruction semantics the user must create the various parts and link them together in a lattice.
 *  The parts are usually built from the bottom up since higher-level parts take lower-level parts as their constructor
 *  arguments: svalue, register state, memory state, RISC operators, and dispatcher.  However, most of the RiscOperators
 *  classes have a default (or mostly default) constructor that builds the prerequisite objects and links them together, so the
 *  only time a user would need to do it explicitly is when they want to mix in a custom part.
 *
 *  @section IS3 Memory Management
 *
 *  Most of the instruction semantics components have abstract base classes. Instances of concrete subclasses thereof are
 *  passed around by pointers, and in order to simplify memory management issues, those objects are reference counted.  Most
 *  objects use boost::shared_ptr, but SValue objects use a faster custom smart pointer (it also uses a custom allocator, and
 *  testing showed a substantial speed improvement over Boost when compiled with GCC's "-O3" switch). In any case, to alleviate
 *  the user from having to remember which kind of objects use which smart pointer implementation, pointer typedefs are created
 *  for each class&mdash;their names are the same as the class but suffixed with "Ptr".  Users will almost exclusively work
 *  with pointers to the objects rather than objects themselves. In fact, holding only a normal pointer to an object is a bit
 *  dangerous since the object will be deleted when the last smart pointer disappears.
 *
 *  In order to encourage users to use the provided smart pointers and not allocate semantic objects on the stack, the normal
 *  constructors are protected.  To create a new object from a class name known at compile time, use the static instance()
 *  method which returns a smart pointer. This is how users will typically create the various semantic objects.
 *
 *  @code
 *      // user code
 *      #include <IntervalSemantics.h>
 *      using namespace rose::BinaryAnalysis::InstructionSemantics2;
 *      BaseSemantics::SValuePtr value = IntervalSemantics::SValue::instance();
 *      // no need to ever delete the object that 'value' points to
 *  @endcode
 *
 *  Most of the semantic objects also provide virtual constructors.  The <code>this</code> pointer is used only to obtain the
 *  dynamic type of the object in order to call the correct virtual constructor. The virtual constructor will create a new
 *  object having the same dynamic type and return a smart pointer to it.  The object on which the virtual constructor is
 *  invoked is a "prototypical object".  For instance, when a RegisterState object is created its constructor is supplied with
 *  a prototypical SValue (a "protoval") which will be used to create new values whenever one is needed (such as when setting
 *  the initial values for the registers).  Virtual constructors are usually named create(), but some classes, particularly
 *  SValue, define other virtual constructors as well. Virtual constructors are most often used when a function overrides a
 *  declaration from a base class, such as when a user defines their own RISC operation:
 *
 *  @code
 *      BaseSemantics::SValuePtr my_accumulate(const BaseSemantics::SValuePtr &operand, int addend) {
 *          BaseSemantics::SValuePtr retval = operand->create(operand->sum + addend);
 *          return retval;
 *      }
 *  @endcode
 *
 *  Some of the semantic objects have a virtual copy constructor named copy().  This operates like a normal copy constructor
 *  but also adjusts reference counts.
 *
 *  @section IS4 Specialization
 *
 *  The instruction semantics architecture is designed to allow users to specialize nearly every part of it.  ROSE defines
 *  triplets (value type, state type, RISC operators) that are designed to work together to implement a particular semantic
 *  domain, but users are free to subclass any of those components to build customized semantic domains.  For example, the x86
 *  simulator (in "projects/simulator") subclasses the PartialSymbolicSemantics state in order to use memory mapped via ROSE's
 *  MemoryMap class, and to handle system calls (among other things).
 *
 *  When writing a subclass the author should implement three versions of each constructor: the real constructor, the static
 *  allocating constructor, and the virtual constructor.  Fortunately, amount amount of extra code needed is not substantial
 *  since the virtual constructor can call the static allocating constructor, which can call the real constructor. The three
 *  versions in more detail are:
 *
 *  1. <i>Real Constructors</i>: These are the normal C++ constructors. They should have protected access and are used
 *     only by authors of subclasses.
 *
 *  2. <i>Static Allocating Constructors</i>: These are class methods that allocate a specific kind of object on the heap and
 *     return a smart pointer to the object.  They are named "instance" to emphasize that they instantiate a new instance of a
 *     particular class and they return the pointer type that is specific to the class (i.e., not one of the BaseSemantics
 *     pointer types).  When an end user constructs a dispatcher, RISC operators, etc., they have particular classes in mind
 *     and use those classes' "instance" methods to create objects.  Static allocating constructors are seldom called by
 *     authors of subclasses; instead the author usually has an object whose provenance can be traced back to a user-created
 *     object (such as a prototypical object), and he invokes one of that object's virtual constructors.
 *
 *  3. <i>Virtual Constructors</i>: A virtual constructor creates a new object having the same run-time type as the object on
 *     which the method is invoked.  Virtual constructors are often named "create" with the virtual copy constructor named
 *     "clone", however the SValue class hierarchy follows a different naming scheme for historic reason--its virtual
 *     constructors end with an underscore.  Virtual constructors return pointer types that defined in BaseSemantics. Subclass
 *     authors usually use this kind of object creation because it frees them from having to know a specific type and allows
 *     their classes to be easily subclassed.
 *
 *  When writing a subclass the author should implement the three versions for each constructor inherited from the super
 *  class. The author may also add any additional constructors that are deemed necessary, realizing that all subclasses of his
 *  class will also need to implement those constructors.
 *
 *  The subclass may define a public virtual destructor that will be called by the smart pointer implementation when the final
 *  pointer to the object is destroyed.
 *
 *  Here is an example of specializing a class that is itself derived from something in ROSE semantics framework.
 *
 *  @code
 *      // Smart pointer for the subclass
 *      typedef boost::shared_ptr<class MyThing> MyThingPtr;
 *
 *      // Class derived from OtherThing, which eventually derives from a class
 *      // defined in BinarySemantics::InstructionSemantics2::BaseSemantics--lets
 *      // say BaseSemantics::Thing -- a non-existent class that follows the rules
 *      // outlined above.
 *      class MyThing: public OtherThing {
 *      private:
 *          char *data; // some data allocated on the heap w/out a smart pointer
 *
 *          // Real constructors.  Normally this will be all the same constructors as
 *          // in the super class, and possibly a few new ones.  Thus anything you add
 *          // here will need to also be implemented in all subclasses hereof. Lets
 *          // pretend that the super class has two constructors: a copy constructor
 *          // and one that takes a pointer to a register state.
 *      protected:
 *          explicit MyThing(const BaseSemantics::RegisterStatePtr &rstate)
 *              : OtherThing(rstate), data(NULL) {}
 *
 *          MyThing(const MyThing &other)
 *              : OtherThing(other), data(copy_string(other.data)) {}
 *
 *          // Define the virtual destructor if necessary.  This won't be called until
 *          // the last smart pointer reference to this object is destroyed.
 *      public:
 *          virtual ~MyThing() {
 *              delete data;
 *          }
 *
 *          // Static allocating constructors. One static allocating constructor
 *          // for each real constructor, including the copy constructor.
 *      public:
 *          static MyThingPtr instance(const BaseSemantics::RegisterStatePtr &rstate) {
 *              return MyThingPtr(new MyThing(rstate));
 *          }
 *
 *          static MyThingPtr instance(const MyThingPtr &other) {
 *              return MyThingPtr(new MyThing(*other));
 *          }
 *
 *          // Virtual constructors. One virtual constructor for each static allocating
 *          // constructor.  It is of utmost importance that we cover all the virtual
 *          // constructors from the super class. These return the most super type
 *          // possible, usually something from BaseSemantics.
 *      public:
 *          virtual BaseSemantics::ThingPtr create(const BaseSemantics::RegisterStatePtr &rstate) {
 *              return instance(rstate);
 *          }
 *
 *          // Name the virtual copy constructor "clone" rather than "create".
 *          virtual BaseSemantics::ThingPtr clone(const BaseSemantics::ThingPtr &other_) {
 *              MyThingPtr other = MyThing::promote(other_);
 *              return instance(other);
 *          }
 *
 *          // Define the checking dynamic pointer cast.
 *      public:
 *          static MyThingPtr promomte(const BaseSemantics::ThingPtr &obj) {
 *              MyThingPtr retval = boost::dynamic_pointer_cast<MyThingPtr>(obj);
 *              assert(retval!=NULL);
 *              return NULL;
 *          }
 *
 *          // Define the methods you need for this class.
 *      public:
 *          virtual char *get_data() const {
 *              return data; // or maybe return a copy in case this gets deleted?
 *          }
 *          virtual void set_data(const char *s) {
 *              data = copy_string(s);
 *          }
 *      private:
 *          void char *copy_string(const char *s) {
 *              if (s==NULL)
 *                  return NULL;
 *              char *retval = new char[strlen(s)+1];
 *              strcpy(retval, s);
 *              return retval;
 *          }
 *     };
 *  @endcode
 *
 *  @section IS5 Other major changes
 *
 *  The new API exists in the rose::BinaryAnalysis::InstructionSemantics2 name space and can coexist with the original API in
 *  rose::BinaryAnalysis::InstructionSemantics&mdash;a program can use both APIs at the same time.
 *
 *  The mapping of class names (and some method) from old API to new API is:
 *  <ul>
 *    <li>ValueType is now called SValue, short for "semantic value".</li>
 *    <li>ValueType::is_known() is now called SValue::is_number() and ValueType::known_value() is now SValue::get_number().
 *    <li>Policy is now called RiscOperators.</li>
 *    <li>X86InstructionSemantics is now called "DispatcherX86.</li>
 *  </ul>
 *
 *  The biggest difference between the APIs is that almost everything in the new API is allocated on the heap and passed by
 *  pointer instead of being allocated on the stack and passed by value.  However, when converting from old API to new API, one
 *  does not need to add calls to delete objects since this happens automatically.
 *
 *  The dispatchers are table driven rather than having a giant "switch" statement.  While nothing prevents a user from
 *  subclassing a dispatcher to override its processInstruction() method, its often easier to just allocate a new instruction
 *  handler and register it with the dispatcher.  This also makes it easy to add semantics for instructions that we hadn't
 *  considered in the original design. See DispatcherX86 for some examples.
 *
 *  The interface between RiscOperators and either MemoryState or RegisterState has been formalized somewhat. See documentation
 *  for RiscOperators::readMemory() and RiscOperators::readRegister().
 *
 *  @section IS6 Future work
 *
 *  <em>Floating-point instructions.</em> Floating point registers are defined in the various RegisterDictionary objects but
 *  none of the semantic states actually define space for them, and we haven't defined any floating-point RISC operations for
 *  policies to implement.  As for existing machine instructions, the dispatchers will translate machine floating point
 *  instructions to RISC operations, and the specifics of those operations will be defined by the various semantic policies.
 *  For instance, the RISC operators for a concrete semantic domain might use the host machine's native IEEE floating point to
 *  emulate the target machine's floating-point operations.
 *
 *  @section IS7 Example
 *
 *  See actual source code for examples since this interface is an active area of ROSE development (as of Jan-2013). The
 *  tests/roseTests/binaryTests/semanticSpeed.C has very simple examples for a variety of semantic domains. In order to use one
 *  of ROSE's predefined semantic domains you'll likely need to define some types and variables. Here's what the code would
 *  look like when using default components of the Symbolic domain:
 *
 *  @code
 *   // New API 
 *   using namespace rose::BinaryAnalysis::InstructionSemantics2;
 *   BaseSemantics::RiscOperatorsPtr operators = SymbolicSemantics::RiscOperators::instance();
 *   BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(operators);
 *
 *   // Old API for comparison
 *   using namespace rose::BinaryAnalysis::InstructionSemantics;
 *   typedef SymbolicSemantics::Policy<> Policy;
 *   Policy policy;
 *   X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> semantics(policy);
 *  @endcode
 *
 *  And here's almost the same example but explicitly creating all the parts. Normally you'd only write it this way if you were
 *  replacing one or more of the parts with your own class, so we'll use MySemanticValue as the semantic value type:
 *
 *  @code
 *   // New API, constructing the lattice from bottom up.
 *   // Almost copied from SymbolicSemantics::RiscOperators::instance()
 *   using namespace rose::BinaryAnalysis::InstructionSemantics2;
 *   BaseSemantics::SValuePtr protoval = MySemanticValue::instance();
 *   BaseSemantics::RegisterStatePtr regs = BaseSemantics::RegisterStateX86::instance(protoval);
 *   BaseSemantics::MemoryStatePtr mem = SymbolicSemantics::MemoryState::instance(protoval);
 *   BaseSemantics::StatePtr state = BaseSemantics::State::instance(regs, mem);
 *   BaseSemantics::RiscOperatorsPtr operators = SymbolicSemantics::RiscOperators::instance(state);
 *
 *   // The old API was a bit more concise for the user, but was not able to override all the
 *   // components as easily, and the implementation of MySemanticValue would certainly have been
 *   // more complex, not to mention that it wasn't even possible for end users to always correctly
 *   // override a particular method by subclassing.
 *   using namespace rose::BinaryAnalysis::InstructionSemantics;
 *   typedef SymbolicSemantics::Policy<SymbolicSemantics::State, MySemanticValue> Policy;
 *   Policy policy;
 *   X86InstructionSemantics<Policy, MySemanticValue> semantics(policy);
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
namespace InstructionSemantics2 {

/** Diagnostics logging facility for instruction semantics. */
extern Sawyer::Message::Facility mlog;

/** Initialize diagnostics for instruction semantics. */
void initDiagnostics();

/** Base classes for instruction semantics.  Basically, anything that is common to two or more instruction semantic
 *  domains will be factored out and placed in this name space. */
namespace BaseSemantics {

class RiscOperators;

/** Format for printing things. Some semantic domains may want to pass some additional information to print methods on a
 *  per-call basis.  This base class provides something they can subclass to do that.  A reference is passed to all print()
 *  methods for semantic objects. */
class Formatter {
public:
    Formatter(): regdict(NULL), suppress_initial_values(false), indentation_suffix("  "), show_latest_writers(true) {}
    virtual ~Formatter() {}

    /** The register dictionary which is used for printing register names.
     * @{ */
    RegisterDictionary *get_register_dictionary() const { return regdict; }
    void set_register_dictionary(RegisterDictionary *rd) { regdict = rd; }
    /** @} */

    /** Whether register initial values should be suppressed.  If a register's value has a comment that is equal to the
     * register name with "_0" appended, then that value is assumed to be the register's initial value.
     * @{ */
    bool get_suppress_initial_values() const { return suppress_initial_values; }
    void set_suppress_initial_values(bool b=true) { suppress_initial_values=b; }
    void clear_suppress_initial_values() { set_suppress_initial_values(false); }
    /** @} */

    /** The string to print at the start of each line. This only applies to objects that occupy more than one line.
     * @{ */
    std::string get_line_prefix() const { return line_prefix; }
    void set_line_prefix(const std::string &s) { line_prefix = s; }
    /** @} */

    /** Indentation string appended to the line prefix for multi-level, multi-line outputs.
     * @{ */
    std::string get_indentation_suffix() const { return indentation_suffix; }
    void set_indentation_suffix(const std::string &s) { indentation_suffix = s; }
    /** @} */

    /** Whether to show latest writer information for register and memory states.
     * @{ */
    bool get_show_latest_writers() const { return show_latest_writers; }
    void set_show_latest_writers(bool b=true) { show_latest_writers = b; }
    void clear_show_latest_writers() { show_latest_writers = false; }
    /** @} */

protected:
    RegisterDictionary *regdict;
    bool suppress_initial_values;
    std::string line_prefix;
    std::string indentation_suffix;
    bool show_latest_writers;
};

/** Adjusts a Formatter for one additional level of indentation.  The formatter's line prefix is adjusted by appending the
 * formatter's indentation suffix.  When this Indent object is destructed, the formatter's line prefix is reset to its original
 * value. */
class Indent {
private:
    Formatter &fmt;
    std::string old_line_prefix;
public:
    Indent(Formatter &fmt): fmt(fmt) {
        old_line_prefix = fmt.get_line_prefix();
        fmt.set_line_prefix(old_line_prefix + fmt.get_indentation_suffix());
    }
    ~Indent() {
        fmt.set_line_prefix(old_line_prefix);
    }
};

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

/** Base class for exceptions thrown by instruction semantics. */
class Exception: public std::runtime_error {
public:
    SgAsmInstruction *insn;
    Exception(const std::string &mesg, SgAsmInstruction *insn): std::runtime_error(mesg), insn(insn) {}
    void print(std::ostream&) const;
};

/*******************************************************************************************************************************
 *                                      Semantic Values
 *******************************************************************************************************************************/

// This is leftover for compatibility with an older API.  The old API had code like this:
//    User::SValue user_svalue = BaseSemantics::dynamic_pointer_cast<User::SValue>(base_svalue);
// Which can be replaced now with
//    User::SValue user_svalue = base_svalue.dynamicCast<User::SValue>();
template<class To, class From>
Sawyer::SharedPointer<To> dynamic_pointer_cast(const Sawyer::SharedPointer<From> &from) {
    return from.template dynamicCast<To>();
}

/** Smart pointer to an SValue object. SValue objects are reference counted and should not be explicitly deleted.
 *
 *  Note: Although most semantic *Ptr types are based on boost::shared_ptr<>, SValuePtr uses Sawyer::SharedPointer which is
 *  substantially faster. */
typedef Sawyer::SharedPointer<class SValue> SValuePtr;

/** Base class for semantic values.
 *
 *  A semantic value represents a datum from the specimen being analyzed. The datum could be from memory, it could be something
 *  stored in a register, it could be the result of some computation, etc.  The datum in the specimen has a datum type that
 *  might be only partially known; the datum value could, for instance, be 32-bits but unknown whether it is integer or
 *  floating point.
 *
 *  The various semantic domains will define SValue subclasses that are appropriate for that domain--a concrete domain will
 *  define an SValue that specimen data in a concrete form, an interval domain will define an SValue that represents specimen
 *  data in intervals, etc.
 *
 *  Semantics value objects are allocated on the heap and reference counted.  The BaseSemantics::SValue is an abstract class
 *  that defines the interface.  See the rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together.*/
class SValue: public Sawyer::SharedObject, public Sawyer::SharedFromThis<SValue>, public Sawyer::SmallObject {
protected:
    size_t width;                               /** Width of the value in bits. Typically (not always) a power of two. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
protected:
    explicit SValue(size_t nbits): width(nbits) {}  // hot
    SValue(const SValue &other): width(other.width) {}

public:
    virtual ~SValue() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating static constructor.  None are needed--this class is abstract.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating virtual constructors.  undefined_() needs underscores, so we do so consistently for all
    // these allocating virtual c'tors.  However, we use copy() rather than copy_() because this one is fundamentally
    // different: the object (this) is use for more than just selecting which virtual method to invoke.
    //
    // The naming scheme we use here is a bit different than for most other objects for historical reasons.  Most other classes
    // use "create" and "clone" as the virtual constructor names, but SValue uses names ending in undercore, and "copy". The
    // other difference (at least in this base class) is that we don't define any real constructors or static allocating
    // constructors (usually named "instance")--it's because this is an abstract class.
public:
    /** Create a new undefined semantic value.  The new semantic value will have the same dynamic type as the value
     *  on which this virtual method is called.  This is the most common way that a new value is created. */
    virtual SValuePtr undefined_(size_t nbits) const = 0; // hot

    /** Create a new concrete semantic value. The new value will represent the specified concrete value and have the same
     *  dynamic type as the value on which this virtual method is called. This is the most common way that a new constant is
     *  created.  The @p number is truncated to contain @p nbits bits (higher order bits are cleared). */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const = 0; // hot

    /** Create a new, Boolean value. The new semantic value will have the same dynamic type as the value on
     *  which this virtual method is called. This is how 1-bit flag register values (among others) are created. The base
     *  implementation uses number_() to construct a 1-bit value whose bit is zero (false) or one (true). */
    virtual SValuePtr boolean_(bool value) const { return number_(1, value?1:0); }

    /** Create a new value from an existing value, changing the width if @p new_width is non-zero. Increasing the width
     *  logically adds zero bits to the most significant side of the value; decreasing the width logically removes bits from the
     *  most significant side of the value. */
    virtual SValuePtr copy(size_t new_width=0) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-ops since this is the base class
public:
    static SValuePtr promote(const SValuePtr &x) {
        ASSERT_not_null(x);
        return x;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...
public:
    /** Determines if the value is a concrete number. Concrete numbers can be created with the number_(), boolean_()
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

    /** Returns true if concrete non-zero. This is not virtual since it can be implemented in terms of @ref is_number and @ref
     *  get_number. */
    bool isTrue() const {
        return is_number() && get_number()!=0;
    }

    /** Returns true if concrete zero.  This is not virtual since it can be implemented in terms of @ref is_number and @ref
     *  get_number. */
    bool isFalse() const {
        return is_number() && get_number()==0;
    }

    /** Print a value to a stream using default format. The value will normally occupy a single line and not contain leading
     *  space or line termination.  See also, with_format().
     *  @{ */
    void print(std::ostream &stream) const { Formatter fmt; print(stream, fmt); }
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** SValue with formatter. See with_formatter(). */
    class WithFormatter {
        SValuePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const SValuePtr &svalue, Formatter &fmt): obj(svalue), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing values with formatting. The usual way to use this is:
     * @code
     *  SValuePtr val = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*val+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(SValuePtr(this), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
    
    /** Some subclasses support the ability to add comments to values. We define no-op versions of these methods here
     *  because it makes things easier.  The base class tries to be as small as possible by not storing comments at
     *  all. Comments should not affect any computation (comparisons, hash values, etc), and therefore are allowed to be
     *  modified even for const objects.
     * @{ */
    virtual std::string get_comment() const { return ""; }
    virtual void set_comment(const std::string&) const {} // const is intended; cf. doxygen comment
    /** @} */
};



/*******************************************************************************************************************************
 *                                      Register States
 *******************************************************************************************************************************/

/** Smart pointer to a RegisterState object.  RegisterState objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** The set of all registers and their values. RegisterState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::RegisterState is an abstract class that defines the interface.  See the
 *  rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.*/
class RegisterState: public boost::enable_shared_from_this<RegisterState> {
protected:
    SValuePtr protoval;                         /**< Prototypical value for virtual constructors. */
    const RegisterDictionary *regdict;          /**< Registers that are able to be stored by this state. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RegisterState(const SValuePtr &protoval, const RegisterDictionary *regdict)
        : protoval(protoval), regdict(regdict) {
        ASSERT_not_null(protoval);
    }

public:
    virtual ~RegisterState() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None are needed--this class is abstract.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.
public:
    /** Virtual constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be used only
     *  to create additional instances of the value via its virtual constructors.  The prototypical value is normally of the
     *  same type for all parts of a semantic analysis. The register state must be compatible with the rest of the binary
     *  analysis objects in use. */
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const = 0;

    /** Make a copy of this register state. */
    virtual RegisterStatePtr clone() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static RegisterStatePtr promote(const RegisterStatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** The register dictionary should be compatible with the register dictionary used for other parts of binary analysis. At
     *  this time (May 2013) the dictionary is only used when printing.
     * @{ */
    const RegisterDictionary *get_register_dictionary() const { return regdict; }
    void set_register_dictionary(const RegisterDictionary *rd) { regdict = rd; }
    /** @} */

    /** Removes stored values from the register state.
     *
     *  Depending on the register state implementation, this could either store new, distinct undefined values in each
     *  register, or it could simply erase all information about stored values leaving the register state truly empty. For
     *  instance, @ref RegisterStateX86, which stores register values using fixed length arrays assigns new undefined values to
     *  each element of those arrays, whereas RegisterStateGeneric, which uses variable length arrays to store information
     *  about a dynamically changing set of registers, clears its arrays to zero length.
     *
     *  Register states can also be initialized by clearing them or by explicitly writing new values into each desired
     *  register (or both). See @ref RegisterStateGeneric::initialize_nonoverlapping for one way to initialize that register
     *  state. */
    virtual void clear() = 0;

    /** Set all registers to the zero. */
    virtual void zero() = 0;

    /** Read a value from a register. The register descriptor, @p reg, not only describes which register, but also which bits
     * of that register (e.g., "al", "ah", "ax", "eax", and "rax" are all the same hardware register on an amd64, but refer to
     * different parts of that register). The RISC operations are provided so that they can be used to extract the correct bits
     * from a wider hardware register if necessary. See RiscOperators::readRegister() for more details. */
    virtual SValuePtr readRegister(const RegisterDescriptor &reg, RiscOperators *ops) = 0;

    /** Write a value to a register.  The register descriptor, @p reg, not only describes which register, but also which bits
     * of that register (e.g., "al", "ah", "ax", "eax", and "rax" are all the same hardware register on an amd64, but refer to
     * different parts of that register). The RISC operations are provided so that they can be used to insert the @p value bits
     * into a wider the hardware register if necessary. See RiscOperators::readRegister() for more details. */
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops) = 0;

    /** Print the register contents. This emits one line per register and contains the register name and its value.
     *  @{ */
    void print(std::ostream &stream, const std::string prefix="") const {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** RegisterState with formatter. See with_formatter(). */
    class WithFormatter {
        RegisterStatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const RegisterStatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing register states with formatting. The usual way to use this is:
     * @code
     *  RegisterStatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */

};

/** Smart pointer to a RegisterStateGeneric object.  RegisterStateGeneric objects are reference counted and should not be
 *  explicitly deleted. */
typedef boost::shared_ptr<class RegisterStateGeneric> RegisterStateGenericPtr;

/** A RegisterState for any architecture.
 *
 *  This state stores a list of non-overlapping registers and their values, typically only for the registers that have been
 *  accessed.  The state automatically switches between different representations when accessing a register that overlaps with
 *  one or more stored registers.  For instance, if the state stores 64-bit registers and the specimen suddently switches to
 *  32-bit mode, this state will split the 64-bit registers into 32-bit pieces.  If the analysis later returns to 64-bit mode,
 *  the 32-bit pieces are concatenated back to 64-bit values. This splitting and concatenation occurs on a per-register basis
 *  at the time the register is read or written.
 *
 *  The register state also maintains optional information about the most recent writer of each register. The most recent
 *  writer is represented by a virtual address (rose_addr_t) and the addresses are stored at bit resolution--each bit of the
 *  register may have its own writer information. */
class RegisterStateGeneric: public RegisterState {
public:
    // Like a RegisterDescriptor, but only the major and minor numbers.  This state maintains lists of registers, one list per
    // major-minor pair.  When reading or writing a register, the register being accessed is guaranteed to overlap only with
    // those registers on the matching major-minor list, if it overlaps at all.  The lists are typically short (e.g., one list
    // might refer to all the parts of the x86 RAX register, but the RBX parts would be on a different list. None of the
    // registers stored on a particular list overlap with any other register on that same list; when adding new register that
    // would overlap, the registers with which it overlaps must be removed first.
    struct RegStore {
        unsigned majr, minr;
        RegStore(const RegisterDescriptor &d) // implicit
            : majr(d.get_major()), minr(d.get_minor()) {}
        bool operator<(const RegStore &other) const {
            return majr<other.majr || (majr==other.majr && minr<other.minr);
        }
    };
    struct RegPair {
        RegisterDescriptor desc;
        SValuePtr value;
        RegPair(const RegisterDescriptor &desc, const SValuePtr &value): desc(desc), value(value) {}
    };

    typedef std::vector<RegPair> RegPairs;
    typedef Map<RegStore, RegPairs> Registers;

    // A mapping from the bits of a register (e.g., 'al' of 'rax') to the virtual address of the instruction that last wrote
    // a value to those bits.
    typedef RangeMap<Extent, RangeMapNumeric<Extent, rose_addr_t> > WrittenParts;
    typedef Map<RegStore, WrittenParts> WritersMap;
    WritersMap writers;

protected:
    Registers registers;                        /**< Values for registers that have been accessed. */
    bool coalesceOnRead;                        /**< If set, do not modify register representations on readRegister. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RegisterStateGeneric(const SValuePtr &protoval, const RegisterDictionary *regdict)
        : RegisterState(protoval, regdict), coalesceOnRead(true) {
        clear();
    }

    RegisterStateGeneric(const RegisterStateGeneric &other)
        : RegisterState(other), registers(other.registers), coalesceOnRead(true) {
        deep_copy_values();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new register state. The @p protoval argument must be a non-null pointer to a semantic value which will be
     *  used only to create additional instances of the value via its virtual constructors.  The prototypical value is normally
     *  of the same type for all parts of a semantic analysis: its state and operator classes.
     *
     *  The register dictionary, @p regdict, describes the registers that can be stored by this register state, and should be
     *  compatible with the register dictionary used for other parts of binary analysis. */
    static RegisterStateGenericPtr instance(const SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStateGenericPtr(new RegisterStateGeneric(protoval, regdict));
    }

    /** Instantiate a new copy of an existing register state. */
    static RegisterStateGenericPtr instance(const RegisterStateGenericPtr &other) {
        return RegisterStateGenericPtr(new RegisterStateGeneric(*other));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        return instance(protoval, regdict);
    }

    virtual RegisterStatePtr clone() const ROSE_OVERRIDE {
        return RegisterStateGenericPtr(new RegisterStateGeneric(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base register state pointer to a RegisterStateGeneric pointer. This is a checked conversion--it
     *  will fail if @p from does not point to a RegisterStateGeneric object. */
    static RegisterStateGenericPtr promote(const RegisterStatePtr &from) {
        RegisterStateGenericPtr retval = boost::dynamic_pointer_cast<RegisterStateGeneric>(from);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherit
public:
    virtual void clear() ROSE_OVERRIDE;
    virtual void zero() ROSE_OVERRIDE;
    virtual SValuePtr readRegister(const RegisterDescriptor &reg, RiscOperators *ops) ROSE_OVERRIDE;
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops) ROSE_OVERRIDE;
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first defined at this level of the class hierarchy
public:
    /** Initialize all registers of the dictionary.  When the dictionary contains overlapping registers, only the largest
     *  registers are initialized. For example, on a 32-bit x86 architecture EAX would be initialized but not AX, AH, or AL;
     *  requesting AX, AH, or AL will return part of the initial EAX value. */
    virtual void initialize_large();

    /** Initialize all registers of the dictionary.  When the dictionary contains overlapping registers, only the smallest
     *  registers are initialized. For example, on a 32-bit x86 architecture, AX, AH, AL and the non-named high-order 16 bits
     *  of AX are inititialized, but EAX isn't explicitly initialized.  Requesting the value of EAX will return a value
     *  constructed from the various smaller parts. */
    virtual void initialize_small();

    /** Initialize the specified registers of the dictionary.  Each register in the list must not overlap with any other
     *  register in the list, or strange things will happen.  If @p initialize_to_zero is set then the specified registers are
     *  initialized to zero, otherwise they're initialized with the prototypical value's constructor that takes only a size
     *  parameter. This method is somewhat low level and doesn't do much error checking. */
    void initialize_nonoverlapping(const std::vector<RegisterDescriptor>&, bool initialize_to_zero);

    /** Returns the list of all registers and their values.  The returned registers are guaranteed to be non-overlapping,
     * although they might not correspond to actual named machine registers.  For instance, if a 32-bit value was written to
     * the x86 EFLAGS register then the return value will contain a register/value pair for EFLAGS but no pairs for individual
     * flags.  If one subsequently writes a 1-bit value to the ZF flag (bit 6 of EFLAGS) then the return value will contain a
     * register/value pair for ZF, and also a pair for bits 0-5, and a pair for bits 7-31, neither of which correspond to
     * actual register names in x86 (there is no name for bits 0-5 as a whole). The readRegister() and writeRegister() methods
     * can be used to re-cast the various pairs into other groupings; get_stored_registers() is a lower-level interface. */
    virtual RegPairs get_stored_registers() const;

    /** Determines if some of the specified register is stored in the state. Returns true even if only part of the requested
     *  register is in the state (as when one asks about EAX and the state only stores AX). This is slightly more efficient
     *  than calling stored_parts():
     *
     * @code
     *  RegisterStateGenericPtr rstate = ...;
     *  RegisterDescriptor reg = ...;
     *  assert(rstate->partly_exists(reg) == !parts_exist(reg).empty());
     * @endcode
     */
    virtual bool is_partly_stored(const RegisterDescriptor&) const;

    /** Determines if the specified register is wholly stored in the state. Returns if the state contains data for the entire
     *  register, even if that data is split among several smaller parts or exists as a subset of a larger part. */
    virtual bool is_wholly_stored(const RegisterDescriptor&) const;

    /** Determines if the specified register is stored exactly in the state. Returns true only if the specified register wholly
     *  exists and a value can be returned without extracting or concatenating values from larger or smaller stored parts. Note
     *  that a value can also be returned without extracting or conctenating if the state contains no data for the specified
     *  register, as indicated by is_partly_stored() returning false. */
    virtual bool is_exactly_stored(const RegisterDescriptor&) const;

    /** Returns a description of which bits of a register are stored.  The return value is an ExtentMap that contains the bits
     * that are stored in the state. This does not return the value of any parts of stored registers--one gets that with
     * readRegister(). The return value does not contain any bits that are not part of the specified register. */
    virtual ExtentMap stored_parts(const RegisterDescriptor&) const;

    /** Cause a register to not be stored.  Erases all record of the specified register. The RiscOperators pointer is used for
     *  its extract operation if the specified register is not exactly stored in the state, such as if the state
     *  stores RIP and one wants to erase only the 32-bits overlapping with EIP. */
    virtual void erase_register(const RegisterDescriptor&, RiscOperators*);

    /** Functors for traversing register values in a register state. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual SValuePtr operator()(const RegisterDescriptor&, const SValuePtr&) = 0;
    };

    /** Traverse register/value pairs.  Traverses all the (non-overlapping) registers and their values, calling the specified
     *  functor for each register/value pair. If the functor returns a new SValue then the return value becomes the new value
     *  for that register.  The new value must have the same width as the register.
     *
     *  For example, the following code performs a symbolic substitution across all the registers:
     *
     *  @code
     *   struct Substitution: BaseSemantics::RegisterStateGeneric::Visitor {
     *       SymbolicSemantics::SValuePtr from, to;
     *
     *       Substitution(const SymbolicSemantics::SValuePtr &from, const SymbolicSemantics::SValuePtr &to)
     *           : from(from), to(to) {}
     *
     *       BaseSemantics::SValuePtr operator()(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &val_) {
     *           SymbolicSemantics::SValuePtr val = SymbolicSemantics::SValue::promote(val_);
     *           return val->substitute(from, to);
     *       }
     *   };
     *
     *   SymbolicSemantics::SValuePtr original_esp = ...;
     *   SymbolicSemantics::SValuePtr fp = ...; // the frame pointer in terms of original_esp
     *   Substitution subst(original_esp, fp);
     *   RegisterStateGenericPtr regs = ...;
     *   std::cerr <<*regs; // register values before substitution
     *   regs->traverse(subst);
     *   std::cerr <<*regs; // all original_esp have been replaced by fp
     *  @endcode
     *
     * As with most ROSE and STL traversals, the Visitor is not allowed to modify the structure of the object over which it is
     * traversing.  In other words, it's permissible to change the values pointed to by the state, but it is not permissible to
     * perform any operation that might change the list of register parts by adding, removing, or combining parts.  This
     * includes calling readRegister() and writeRegister() except when the register being read or written is already exactly
     * stored in the state as indicated by is_exactly_stored().
     */
    virtual void traverse(Visitor&);

    /** Set the writer for the specified register. Each register (major-minor pair) is able to store a virtual address for each
     *  bit of the register.  By convention, this data member stores the virtual address of the instruction that most recently
     *  wrote a value to those bits. */
    virtual void set_latest_writer(const RegisterDescriptor&, rose_addr_t writer_va);

    /** Clear the writer for the specified register.  Information about the virtual address of the instruction that most
     *  recently wrote a value to the specified register is removed from the register.  The value of the register is not
     *  affected by this call, but the last-writer information is adjusted so it looks like no instruction wrote the bits to
     *  the specified register. See also, set_latest_writer(). */
    virtual void clear_latest_writer(const RegisterDescriptor&);

    /** Clear all information about latest writers for all registers. */
    virtual void clear_latest_writers();

    /** Obtain the set of virtual addresses stored as the latest writers for a register.  A register may have more than one
     *  writer if the register's value was written in parts (such as when requesting the writers for x86 AX when separate
     *  instructions wrote to AL and AH. A register may have no writers if the writer information has been cleared (via
     *  clear_latest_writer()), or no data has ever been written to the register, or data has been written but no writer was
     *  specified. */
    virtual std::set<rose_addr_t> get_latest_writers(const RegisterDescriptor&) const;

    /** Whether reading modifies representation.  When the @ref readRegister method is called to obtain a value for a desired
     *  register that overlaps with some (parts of) registers that already exist in this register state we can proceed in two
     *  ways. In both cases the return value will include data that's already stored, but the difference is in how we store the
     *  returned value in the register state: (1) we can erase the (parts of) existing registers that overlap and store the
     *  desired register and store the returned value so that the register we just read appears as one atomic value, or (2) we
     *  can keep the existing registers and write only those parts of the return value that fall between the gaps.
     *
     *  If the coalesceOnRead property is set, then the returned value is stored atomically even when the value might be a
     *  function of values that are already stored. Otherwise, existing registerss are not rearranged and only those parts of
     *  the return value that fall into the gaps between existing registers are stored.
     *
     *  The set/clear modifiers return the previous value of this property.
     *
     * @{ */
    virtual bool get_coalesceOnRead() { return coalesceOnRead; }
    virtual bool set_coalesceOnRead(bool b=true) { bool retval=coalesceOnRead; coalesceOnRead=b; return retval; }
    virtual bool clear_coalescOnRead() { return set_coalesceOnRead(false); }
    /** @} */

    /** Temporarily turn off coalescing on read.  Original state is restored by the destructor. */
    class NoCoalesceOnRead {
        RegisterStateGeneric *rstate_;
        bool oldValue_;
    public:
        /** Turn off coalesceOnRead for the specified register state. */
        explicit NoCoalesceOnRead(RegisterStateGeneric *rstate): rstate_(rstate), oldValue_(rstate->clear_coalescOnRead()) {}
        ~NoCoalesceOnRead() { rstate_->set_coalesceOnRead(oldValue_); }
    };
    
protected:
    void deep_copy_values();
    static void get_nonoverlapping_parts(const Extent &overlap, const RegPair &rp, RiscOperators *ops,
                                         RegPairs *pairs/*out*/);
};

/** Smart pointer to a RegisterStateX86 object.  RegisterStateX86 objects are reference counted and should not be
 *  explicitly deleted. */
typedef boost::shared_ptr<class RegisterStateX86> RegisterStateX86Ptr;

/** The set of all registers and their values for a 32-bit x86 architecture.
 *
 *  This register state is probably not too useful anymore; use RegisterStateGeneric instead.
 *
 *  The general purpose registers are stored as 32-bit values; subparts thereof will require calls to the RiscOperators
 *  extract() or concat() operators.  The status bits from the EFLAGS register are stored individually since that's how they're
 *  typically accessed; access to the FLAGS/EFLAGS register as a whole will require calls to the RISC operators. */
class RegisterStateX86: public RegisterState {
public:
    static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
    static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
    static const size_t n_flags = 32;           /**< Number of flag registers in this state. */
    static const size_t n_st = 8;               /**< Number of ST registers (not counting _st_top pseudo register). */
    static const size_t n_xmm = 8;              /**< Number f XMM registers. */

    SValuePtr ip;                               /**< Instruction pointer. */
    SValuePtr gpr[n_gprs];                      /**< General-purpose registers */
    SValuePtr segreg[n_segregs];                /**< Segmentation registers. */
    SValuePtr flag[n_flags];                    /**< Control/status flags (i.e., FLAG register). */
    SValuePtr st[n_st];                         /**< Floating point circular stack. */
    SValuePtr fpstatus;                         /**< Floating-point status word. */
    SValuePtr xmm[n_xmm];                       /**< XMM registers. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RegisterStateX86(const SValuePtr &protoval, const RegisterDictionary *regdict): RegisterState(protoval, regdict) {
        clear();
    }

    RegisterStateX86(const RegisterStateX86 &other): RegisterState(other) {
        ip = other.ip->copy();
        for (size_t i=0; i<n_gprs; ++i)
            gpr[i] = other.gpr[i]->copy();
        for (size_t i=0; i<n_segregs; ++i)
            segreg[i] = other.segreg[i]->copy();
        for (size_t i=0; i<n_flags; ++i)
            flag[i] = other.flag[i]->copy();
        for (size_t i=0; i<n_st; ++i)
            st[i] = other.st[i]->copy();
        fpstatus = other.fpstatus;
        for (size_t i=0; i<n_xmm; ++i)
            xmm[i] = other.xmm[i]->copy();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new register state. The @p protoval argument must be a non-null pointer to a semantic value which will be
     *  used only to create additional instances of the value via its virtual constructors.  The prototypical value is normally
     *  of the same type for all parts of a semantic analysis: its state and operator classes. */
    static RegisterStateX86Ptr instance(const SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStateX86Ptr(new RegisterStateX86(protoval, regdict));
    }

    /** Instantiate a new copy of an existing register state. */
    static RegisterStateX86Ptr instance(const RegisterStateX86Ptr &other) {
        return RegisterStateX86Ptr(new RegisterStateX86(*other));
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        return instance(protoval, regdict);
    }

    virtual RegisterStatePtr clone() const ROSE_OVERRIDE {
        return RegisterStatePtr(new RegisterStateX86(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base register state pointer to a RegisterStateX86 pointer. This is a checked conversion--it
     *  will fail if @p from does not point to a RegisterStateX86 object. */
    static RegisterStateX86Ptr promote(const RegisterStatePtr &from) {
        RegisterStateX86Ptr retval = boost::dynamic_pointer_cast<RegisterStateX86>(from);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    virtual void clear() ROSE_OVERRIDE;
    virtual void zero() /* override*/;
    virtual SValuePtr readRegister(const RegisterDescriptor &reg, RiscOperators *ops) ROSE_OVERRIDE;
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops) ROSE_OVERRIDE;
    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
protected:
    // helpers for readRegister()
    virtual SValuePtr readRegisterGpr(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterFlag(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterSeg(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterIp(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterSt(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterXmm(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterFpStatus(const RegisterDescriptor &reg, RiscOperators *ops);

    // helpers for writeRegister()
    virtual void writeRegisterGpr(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterFlag(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterSeg(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterIp(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterSt(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterXmm(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterFpStatus(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);

    // Generate a name for initial values.
    virtual std::string initialValueName(const RegisterDescriptor&) const;
};


/*******************************************************************************************************************************
 *                                      Memory State
 *******************************************************************************************************************************/

/** Smart pointer to a MemoryState object. MemoryState objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Represents all memory in the state. MemoryState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::MemoryState is an abstract class that defines the interface.  See the
 *  rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.*/
class MemoryState: public boost::enable_shared_from_this<MemoryState> {
    SValuePtr addrProtoval_;                            /**< Prototypical value for addresses. */
    SValuePtr valProtoval_;                             /**< Prototypical value for values. */
    ByteOrder::Endianness byteOrder_;                   /**< Memory byte order. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit MemoryState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : addrProtoval_(addrProtoval), valProtoval_(valProtoval), byteOrder_(ByteOrder::ORDER_UNSPECIFIED) {
        ASSERT_not_null(addrProtoval);
        ASSERT_not_null(valProtoval);
    }

    MemoryState(const MemoryStatePtr &other)
        : addrProtoval_(other->addrProtoval_), valProtoval_(other->valProtoval_), byteOrder_(ByteOrder::ORDER_UNSPECIFIED) {}

public:
    virtual ~MemoryState() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors. None needed since this class is abstract

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual allocating constructor.
     *
     *  Allocates and constructs a new MemoryState object having the same dynamic type as this object. A prototypical SValue
     *  must be supplied and will be used to construct any additional SValue objects needed during the operation of a
     *  MemoryState.  Two prototypical values are supplied, one for addresses and another for values stored at those addresses,
     *  although they will almost always be the same. */
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const = 0;

    /** Virtual allocating copy constructor. Creates a new MemoryState object which is a copy of this object. */
    virtual MemoryStatePtr clone() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static MemoryStatePtr promote(const MemoryStatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Return the address protoval.  The address protoval is used to construct other memory addresses via its virtual
     *  constructors. */
    SValuePtr get_addr_protoval() const { return addrProtoval_; }

    /** Return the value protoval.  The value protoval is used to construct other stored values via its virtual
     *  constructors. */
    SValuePtr get_val_protoval() const { return valProtoval_; }

    /** Clear memory. Removes all memory cells from this memory state. */
    virtual void clear() = 0;

    /** Memory byte order.
     *  @{ */
    ByteOrder::Endianness get_byteOrder() const { return byteOrder_; }
    void set_byteOrder(ByteOrder::Endianness bo) { byteOrder_ = bo; }
    /** @} */

    /** Read a value from memory.
     *
     *  Consults the memory represented by this MemoryState object and returns a semantic value. Depending on the semantic
     *  domain, the value can be a value that is already stored in the memory state, a supplied default value, a new value
     *  constructed from some combination of existing values and/or the default value, or anything else.  For instance, in a
     *  symbolic domain the @p address could alias multiple existing memory locations and the implementation may choose to
     *  return a McCarthy expression.  Additional data (such as SMT solvers) may be passed via the RiscOperators argument.
     *
     *  The size of the value being read does not necessarily need to be equal to the size of values stored in the memory
     *  state, though it typically is(1). For instance, an implementation may allow reading a 32-bit little endian value from a
     *  memory state that stores only bytes.  A RiscOperators object is provided for use in these situations.
     *
     *  In order to support cases where an address does not match any existing location, the @p dflt value can be used to
     *  initialize a new memory location.  The manner in which the default is used depends on the implementation.  In any case,
     *  the width of the @p dflt value determines how much to read.
     *
     *  Footnote 1: A MemoryState::readMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::readMemory().  The designers of the MemoryState, State, and RiscOperators subclasses will need to
     *  coordinate to decide which layer should handle concatenating values from individual memory locations. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Write a value to memory.
     *
     *  Consults the memory represented by this MemoryState object and possibly inserts the specified value.  The details of
     *  how a value is inserted into a memory state depends entirely on the implementation in a subclass and will probably be
     *  different for each semantic domain.
     *
     *  A MemoryState::writeMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::writeMemory(). The designers of the MemoryState, State, and RiscOperators will need to coordinate to
     *  decide which layer (if any) should handle splitting a multi-byte value into multiple memory locations. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Print a memory state to more than one line of output.
     * @{ */
    void print(std::ostream &stream, const std::string prefix="") const {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** MemoryState with formatter. See with_formatter(). */
    class WithFormatter {
        MemoryStatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const MemoryStatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing memory states with formatting. The usual way to use this is:
     * @code
     *  MemoryStatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};

/******************************************************************************************************************
 *                                  Cell List Memory State
 ******************************************************************************************************************/

/** Smart pointer to a MemoryCell object.  MemoryCell objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryCell> MemoryCellPtr;

/** Represents one location in memory.
 *
 *  Each memory cell has an address and a value. MemoryCell objects are used by the MemoryCellList to represent a memory
 *  state. */
class MemoryCell: public boost::enable_shared_from_this<MemoryCell> {
    SValuePtr address_;                                 // Address of memory cell.
    SValuePtr value_;                                   // Value stored at that address.
    Sawyer::Optional<rose_addr_t> latestWriter_;        // Optional address for most recent writer of this cell's value.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryCell(const SValuePtr &address, const SValuePtr &value)
        : address_(address), value_(value) {
        ASSERT_not_null(address);
        ASSERT_not_null(value);
    }

    // deep-copy cell list so modifying this new one doesn't alter the existing one
    MemoryCell(const MemoryCell &other) {
        address_ = other.address_->copy();
        value_ = other.value_->copy();
        latestWriter_ = other.latestWriter_;
    }

public:
    virtual ~MemoryCell() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory cell object with the specified address and value. */
    static MemoryCellPtr instance(const SValuePtr &address, const SValuePtr &value) {
        return MemoryCellPtr(new MemoryCell(address, value));
    }

    /** Instantiates a new copy of an existing cell. */
    static MemoryCellPtr instance(const MemoryCellPtr &other) {
        return MemoryCellPtr(new MemoryCell(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Creates a new memory cell object with the specified address and value. */
    virtual MemoryCellPtr create(const SValuePtr &address, const SValuePtr &value) {
        return instance(address, value);
    }

    /** Creates a new deep-copy of this memory cell. */
    virtual MemoryCellPtr clone() const {
        return MemoryCellPtr(new MemoryCell(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static MemoryCellPtr promote(const MemoryCellPtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Accessor for the memory cell address.
     * @{ */
    virtual SValuePtr get_address() const { return address_; }
    virtual void set_address(const SValuePtr &addr) {
        ASSERT_not_null(addr);
        address_ = addr;
    }
    /** @}*/

    /** Accessor for the value stored at a memory location.
     * @{ */
    virtual SValuePtr get_value() const { return value_; }
    virtual void set_value(const SValuePtr &v) {
        ASSERT_not_null(v);
        value_ = v;
    }
    /** @}*/

    /** Accessor for the last writer for a memory location.  Each memory cell is able to store an optional virtual address to
     *  describe the most recent instruction that wrote to this memory location.
     * @{ */
    virtual boost::optional<rose_addr_t> get_latest_writer() const ROSE_DEPRECATED("use latestWriter instead") {
        return latestWriter_ ? boost::optional<rose_addr_t>(*latestWriter_) : boost::optional<rose_addr_t>();
    }
    virtual void set_latest_writer(rose_addr_t writer_va) ROSE_DEPRECATED("use latestWriter instead") {
        latestWriter_ = writer_va;
    }
    virtual void clear_latest_writer() ROSE_DEPRECATED("use clearLatestWriter instead") {
        latestWriter_ = Sawyer::Nothing();
    }

    virtual Sawyer::Optional<rose_addr_t> latestWriter() const { return latestWriter_; }
    virtual void latestWriter(rose_addr_t writerVa) { latestWriter_ = writerVa; }
    virtual void latestWriter(const Sawyer::Optional<rose_addr_t> w) { latestWriter_ = w; }
    virtual void clearLatestWriter() { latestWriter_ = Sawyer::Nothing(); }
    /** @} */

    /** Determines whether two memory cells can alias one another.  Two cells may alias one another if it is possible that
     *  their addresses cause them to overlap.  For cells containing one-byte values, aliasing may occur if their two addresses
     *  may be equal; multi-byte cells will need to check ranges of addresses. */
    virtual bool may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;

    /** Determines whether two memory cells must alias one another.  Two cells must alias one another when it can be proven
     * that their addresses cause them to overlap.  For cells containing one-byte values, aliasing must occur unless their
     * addresses can be different; multi-byte cells will need to check ranges of addresses. */
    virtual bool must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;
    
    /** Print the memory cell on a single line.
     * @{ */
    void print(std::ostream &stream) const {
        Formatter fmt;
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const;
    /** @} */

    /** State with formatter. See with_formatter(). */
    class WithFormatter {
        MemoryCellPtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const MemoryCellPtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing states with formatting. The usual way to use this is:
     * @code
     *  MemoryCellPtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};

/** Smart pointer to a MemoryCell object. MemoryCell objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryCellList> MemoryCellListPtr;

/** Simple list-based memory state.
 *
 *  MemoryCellList uses a list of MemoryCell objects to represent the memory state. Each memory cell contains at least an
 *  address and a value, both of which have a run-time width.  The default MemoryCellList configuration restricts memory cell
 *  values to be one byte wide and requires the caller to perform any necessary byte extraction or concatenation when higher
 *  software layers are reading/writing multi-byte values.  Using one-byte values simplifies the aliasing calculations.
 *  However, this class defines a @p byte_restricted property that can be set to false to allow the memory to store
 *  variable-width cell values.
 *
 *  MemoryCellList also provides a scan() method that returns a list of memory cells that alias a specified address. This
 *  method can be used by a higher-level readMemory() operation in preference to the usuall MemoryState::readMemory().
 *
 *  There is no requirement that a State use a MemoryCellList as its memory state; it can use any subclass of MemoryState.
 *  Since MemoryCellList is derived from MemoryState it must provide virtual allocating constructors, which makes it possible
 *  for users to define their own subclasses and use them in the semantic framework.
 *
 *  This implementation stores memory cells in reverse chronological order: the most recently created cells appear at the
 *  beginning of the list.  Subclasses, of course, are free to reorder the list however they want. */
class MemoryCellList: public MemoryState {
public:
    typedef std::list<MemoryCellPtr> CellList;
protected:
    MemoryCellPtr protocell;                    // prototypical memory cell used for its virtual constructors
    CellList cells;                             // list of cells in reverse chronological order
    bool byte_restricted;                       // are cell values all exactly one byte wide?
    MemoryCellPtr latest_written_cell;          // the cell whose value was most recently written to, if any

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit MemoryCellList(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()),
          protocell(protocell),
          byte_restricted(true) {
        ASSERT_not_null(protocell);
        ASSERT_not_null(protocell->get_address());
        ASSERT_not_null(protocell->get_value());
    }

    MemoryCellList(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval),
          protocell(MemoryCell::instance(addrProtoval, valProtoval)),
          byte_restricted(true) {}

    // deep-copy cell list so that modifying this new state does not modify the existing state
    MemoryCellList(const MemoryCellList &other)
        : MemoryState(other), protocell(other.protocell), byte_restricted(other.byte_restricted) {
        for (CellList::const_iterator ci=other.cells.begin(); ci!=other.cells.end(); ++ci)
            cells.push_back((*ci)->clone());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical memory state. This constructor uses the default type for the cell type (based on the
     *  semantic domain). The prototypical values are usually the same (addresses and stored values are normally the same
     *  type). */
    static MemoryCellListPtr instance(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) {
        return MemoryCellListPtr(new MemoryCellList(addrProtoval, valProtoval));
    }
    
    /** Instantiate a new memory state with prototypical memory cell. */
    static MemoryCellListPtr instance(const MemoryCellPtr &protocell) {
        return MemoryCellListPtr(new MemoryCellList(protocell));
    }

    /** Instantiate a new copy of an existing memory state. */
    static MemoryCellListPtr instance(const MemoryCellListPtr &other) {
        return MemoryCellListPtr(new MemoryCellList(*other));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }
    
    /** Virtual allocating constructor. */
    virtual MemoryStatePtr create(const MemoryCellPtr &protocell) const {
        return instance(protocell);
    }

    virtual MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryCellList(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base memory state pointer to a BaseSemantics::MemoryCellList pointer. The memory state @p m must have
     *  a BaseSemantics::MemoryCellList dynamic type. */
    static MemoryCellListPtr promote(const BaseSemantics::MemoryStatePtr &m) {
        MemoryCellListPtr retval = boost::dynamic_pointer_cast<MemoryCellList>(m);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    virtual void clear() ROSE_OVERRIDE {
        cells.clear();
        latest_written_cell.reset();
    }

    /** Read a value from memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation scans the reverse chronological cell list until
     *  it finds a cell that must alias the specified addresses and value size. Along the way, it accumulates a list of cells
     *  that may alias the specified address.  If the accumulated list does not contain exactly one cell, or the scan fell off
     *  the end of the list, then @p dflt becomes the return value, otherwise the return value is the single value on the
     *  accumulated list. If the @p dflt value is returned, then it is also pushed onto the front of the cell list.
     *
     *  The width of the @p dflt value determines how much data is read. The base implementation assumes that all cells contain
     *  8-bit values. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    /** Write a value to memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation creates a new memory cell and pushes it onto
     *  the front of the cell list.
     *
     *  The base implementation assumes that all cells contain 8-bit values. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Indicates whether memory cell values are required to be eight bits wide.  The default is true since this simplifies the
     * calculations for whether two memory cells are alias and how to combine the value from two or more aliasing cells. A
     * memory that contains only eight-bit values requires that the caller concatenate/extract individual bytes when
     * reading/writing multi-byte values.
     * @{ */
    virtual bool get_byte_restricted() const { return byte_restricted; }
    virtual void set_byte_restricted(bool b) { byte_restricted = b; }
    /** @} */

    /** Scans the cell list and returns entries that may alias the given address and value size. The scanning starts at the
     *  beginning of the list (which is normally stored in reverse chronological order) and continues until it reaches either
     *  the end, or a cell that must alias the specified address. If the last cell in the returned list must alias the
     *  specified address, then true is returned via @p short_circuited argument. */
    virtual CellList scan(const BaseSemantics::SValuePtr &address, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps,
                          bool &short_circuited/*out*/) const;

    /** Visitor for traversing a cell list. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual void operator()(MemoryCellPtr&) = 0;
    };

    /** Visit each memory cell. */
    void traverse(Visitor &visitor);

    /** Returns the list of all memory cells.
     * @{ */
    virtual const CellList& get_cells() const { return cells; }
    virtual       CellList& get_cells()       { return cells; }
    /** @} */

    /** Returns the cell most recently written. */
    virtual MemoryCellPtr get_latest_written_cell() const { return latest_written_cell; }

    /** Returns the union of writer virtual addresses for cells that may alias the given address. */
    virtual std::set<rose_addr_t> get_latest_writers(const SValuePtr &addr, size_t nbits,
                                                     RiscOperators *addrOps, RiscOperators *valOps);
};

/******************************************************************************************************************
 *                                      State
 ******************************************************************************************************************/

/** Smart pointer to a State object.  State objects are reference counted and should not be explicitly deleted. */
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
 *  the interface.  See the rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit
 *  together.  */
class State: public boost::enable_shared_from_this<State> {
protected:
    SValuePtr protoval;                         /**< Initial value used to create additional values as needed. */
    RegisterStatePtr registers;                 /**< All machine register values for this semantic state. */
    MemoryStatePtr  memory;                     /**< All memory for this semantic state. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    State(const RegisterStatePtr &registers, const MemoryStatePtr &memory)
        : registers(registers), memory(memory) {
        ASSERT_not_null(registers);
        ASSERT_not_null(memory);
        protoval = registers->get_protoval();
        ASSERT_not_null(protoval);
    }

    // deep-copy the registers and memory
    State(const State &other)
        : protoval(other.protoval) {
        registers = other.registers->clone();
        memory = other.memory->clone();
    }

public:
    virtual ~State() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new state object with specified register and memory states. */
    static StatePtr instance(const RegisterStatePtr &registers, const MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }

    /** Instantiate a new copy of an existing state. */
    static StatePtr instance(const StatePtr &other) {
        return StatePtr(new State(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. */
    virtual StatePtr create(const RegisterStatePtr &registers, const MemoryStatePtr &memory) const {
        return instance(registers, memory);
    }

    /** Virtual copy constructor. Allocates a new state object which is a deep copy of this state. States must be copyable
     *  objects because many analyses depend on being able to make a copy of the entire semantic state at each machine
     *  instruction, at each CFG vertex, etc. */
    virtual StatePtr clone() const {
        StatePtr self = boost::const_pointer_cast<State>(shared_from_this());
        return instance(self);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static StatePtr promote(const StatePtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other methods that are part of our API. Most of these just chain to either the register state and/or the memory state.
public:
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

    /** Return the register state. */
    RegisterStatePtr get_register_state() {
        return registers;
    }

    /** Return the memory state. */
    MemoryStatePtr get_memory_state() {
        return memory;
    }
    
    /** Read a value from a register.
     *
     *  The BaseSemantics::readRegister() implementation simply delegates to the register state member of this state. See
     *  BaseSemantics::RiscOperators::readRegister() for details. */
    virtual SValuePtr readRegister(const RegisterDescriptor &desc, RiscOperators *ops) {
        return registers->readRegister(desc, ops);
    }

    /** Write a value to a register.
     *
     *  The BaseSemantics::readRegister() implementation simply delegates to the register state member of this state. See
     *  BaseSemantics::RiscOperators::writeRegister() for details. */
    virtual void writeRegister(const RegisterDescriptor &desc, const SValuePtr &value, RiscOperators *ops) {
        registers->writeRegister(desc, value, ops);
    }

    /** Read a value from memory.
     *
     *  The BaseSemantics::readMemory() implementation simply delegates to the memory state member of this state. See
     *  BaseSemantics::RiscOperators::readMemory() for details.  */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) {
        return memory->readMemory(address, dflt, addrOps, valOps);
    }

    /** Write a value to memory.
     *
     *  The BaseSemantics::writeMemory() implementation simply delegates to the memory state member of this state. See
     *  BaseSemantics::RiscOperators::writeMemory() for details. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) {
        memory->writeMemory(addr, value, addrOps, valOps);
    }

    /** Print the register contents. This emits one line per register and contains the register name and its value.
     * @{ */
    void print_registers(std::ostream &stream, const std::string prefix="") {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print_registers(stream, fmt);
    }
    virtual void print_registers(std::ostream &stream, Formatter &fmt) const {
        registers->print(stream, fmt);
    }
    /** @} */

    /** Print memory contents.  This simply calls the MemoryState::print method.
     * @{ */
    void print_memory(std::ostream &stream, const std::string prefix="") const {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print_registers(stream, fmt);
    }
    virtual void print_memory(std::ostream &stream, Formatter &fmt) const {
        memory->print(stream, fmt);
    }
    /** @} */

    /** Print the state.  This emits a multi-line string containing the registers and all known memory locations.
     * @{ */
    void print(std::ostream &stream, const std::string prefix="") const {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const;
    /** @} */

    /** State with formatter. See with_formatter(). */
    class WithFormatter {
        StatePtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const StatePtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing states with formatting. The usual way to use this is:
     * @code
     *  StatePtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};

/******************************************************************************************************************
 *                                  RISC Operators
 ******************************************************************************************************************/

/** Smart pointer to a RiscOperator object. RiscOperator objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Base class for most instruction semantics RISC operators.  This class is responsible for defining the semantics of the
 *  RISC-like operations invoked by the translation object (e.g., X86InstructionSemantics).  We omit the definitions for most
 *  of the RISC operations from the base class so that failure to implement them in a subclass is an error.
 *
 *  RISC operator arguments are, in general, SValue pointers.  However, if the width of a RISC operator's result depends on an
 *  argument's value (as opposed to depending on the argument width), then that argument must be a concrete value (i.e., an
 *  integral type).  This requirement is due to the fact that SMT solvers need to know the sizes of their bit
 *  vectors. Operators extract(), unsignedExtend(), signExtend(), readRegister(), and readMemory() fall into this category.
 *
 *  RiscOperator objects are allocated on the heap and reference counted.  The BaseSemantics::RiscOperator is an abstract class
 *  that defines the interface.  See the rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together. */
class RiscOperators: public boost::enable_shared_from_this<RiscOperators> {
protected:
    SValuePtr protoval;                         /**< Prototypical value used for its virtual constructors. */
    StatePtr state;                             /**< State upon which RISC operators operate. */
    SgAsmInstruction *cur_insn;                 /**< Current instruction, as set by latest startInstruction() call. */
    size_t ninsns;                              /**< Number of instructions processed. */
    SMTSolver *solver;                          /**< Optional SMT solver. */
    std::string name;                           /**< Name to use for debugging. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit RiscOperators(const SValuePtr &protoval, SMTSolver *solver=NULL)
        : protoval(protoval), cur_insn(NULL), ninsns(0), solver(solver) {
        ASSERT_not_null(protoval);
    }

    explicit RiscOperators(const StatePtr &state, SMTSolver *solver=NULL)
        : state(state), cur_insn(NULL), ninsns(0), solver(solver) {
        ASSERT_not_null(state);
        protoval = state->get_protoval();
    }

public:
    virtual ~RiscOperators() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None needed since this class is abstract.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors.
public:
    /** Virtual allocating constructor.  The @p protoval is a prototypical semantic value that is used as a factory to create
     *  additional values as necessary via its virtual constructors.  The state upon which the RISC operations operate must be
     *  provided by a separate call to the set_state() method. An optional SMT solver may be specified (see set_solver()). */
    virtual RiscOperatorsPtr create(const SValuePtr &protoval, SMTSolver *solver=NULL) const = 0;

    /** Virtual allocating constructor.  The supplied @p state is that upon which the RISC operations operate and is also used
     *  to define the prototypical semantic value. Other states can be supplied by calling set_state(). The prototypical
     *  semantic value is used as a factory to create additional values as necessary via its virtual constructors. An optional
     *  SMT solver may be specified (see set_solver()). */
    virtual RiscOperatorsPtr create(const StatePtr &state, SMTSolver *solver=NULL) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts.  No-op since this is the base class.
public:
    static RiscOperatorsPtr promote(const RiscOperatorsPtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Other methods part of our API
public:
    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    virtual SValuePtr get_protoval() const { return protoval; }

    /** Sets the satisfiability modulo theory (SMT) solver to use for certain operations.  An SMT solver is optional and not
     *  all semantic domains will make use of a solver.  Domains that use a solver will fall back to naive implementations when
     *  a solver is not available (for instance, equality of two values might be checked by looking at whether the values are
     *  identical).  */
    virtual void set_solver(SMTSolver *solver) { this->solver = solver; }

    /** Returns the solver that is currently being used.  A null return value means that no SMT solver is being used and that
     *  certain operations are falling back to naive implementations. */
    virtual SMTSolver *get_solver() const { return solver; }

    /** Access the state upon which the RISC operations operate. The state need not be set until the first instruction is
     *  executed (and even then, some RISC operations don't need any machine state (typically, only register and memory read
     *  and write operators need state).  Different state objects can be swapped in at pretty much any time.  Modifying the
     *  state has no effect on this object's prototypical value which was initialized by the constructor; new states should
     *  have a prototyipcal value of the same dynamic type.
     * @{ */
    virtual StatePtr get_state() const { return state; }
    virtual void set_state(const StatePtr &s) { state = s; }
    /** @} */

    /** A name used for debugging.
     * @{ */
    virtual const std::string& get_name() const { return name; }
    virtual void set_name(const std::string &s) { name = s; }
    /** @} */

    /** Print multi-line output for this object.
     * @{ */
    void print(std::ostream &stream, const std::string prefix="") const {
        Formatter fmt;
        fmt.set_line_prefix(prefix);
        print(stream, fmt);
    }
    virtual void print(std::ostream &stream, Formatter &fmt) const {
        state->print(stream, fmt);
    }
    /** @} */

    /** RiscOperators with formatter. See with_formatter(). */
    class WithFormatter {
        RiscOperatorsPtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const RiscOperatorsPtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing RISC operators with formatting. The usual way to use this is:
     * @code
     *  RiscOperatorsPtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */

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
    virtual void startInstruction(SgAsmInstruction *insn);

    /** Called at the end of every instruction.  This method is invoked whenever the translation object ends processing for an
     *  instruction.  This is not called if there's an exception during processing. */
    virtual void finishInstruction(SgAsmInstruction *insn) {
        ASSERT_not_null(insn);
        ASSERT_require(cur_insn==insn);
        cur_insn = NULL;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Value Construction Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The trailing underscores are necessary for for undefined_() on some machines, so we just add one to the end of all the
    // virtual constructors for consistency.

    /** Returns a new undefined value. Uses the prototypical value to virtually construct the new value. */
    virtual SValuePtr undefined_(size_t nbits) {
        return protoval->undefined_(nbits);
    }

    /** Returns a number of the specified bit width.  Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr number_(size_t nbits, uint64_t value) {
        return protoval->number_(nbits, value);
    }

    /** Returns a Boolean value. Uses the prototypical value to virtually construct a new value. */
    virtual SValuePtr boolean_(bool value) {
        return protoval->boolean_(value);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  x86-specific Operations (FIXME)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked to filter call targets.  This method is called whenever the translation object is about to invoke a function
     *  call.  The target address is passed as an argument and a (new) target should be returned. */
    virtual SValuePtr filterCallTarget(const SValuePtr &a) {
        return a->copy();
    }

    /** Invoked to filter return targets.  This method is called whenever the translation object is about to return from a
     *  function call (such as for the x86 "RET" instruction).  The return address is passed as an argument and a (new) return
     *  address should be returned. */
    virtual SValuePtr filterReturnTarget(const SValuePtr &a) {
        return a->copy();
    }

    /** Invoked to filter indirect jumps.  This method is called whenever the translation object is about to unconditionally
     *  jump to a new address (such as for the x86 "JMP" instruction).  The target address is passed as an argument and a (new)
     *  target address should be returned. */
    virtual SValuePtr filterIndirectJumpTarget(const SValuePtr &a) {
        return a->copy();
    }

    /** Invoked for the x86 HLT instruction. */
    virtual void hlt() {}

    /** Invoked for the x86 CPUID instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual void cpuid() {}

    /** Invoked for the x86 RDTSC instruction. FIXME: x86-specific stuff should be in the dispatcher. */
    virtual SValuePtr rdtsc() { return undefined_(64); }


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

    /** Rotate bits to the left. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. The behavior is modulo the width of @p a regardles of whether the implementation makes that a special case or
     *  handles it naturally. */
    virtual SValuePtr rotateLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Rotate bits to the right. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. The behavior is modulo the width of @p a regardles of whether the implementation makes that a special case or
     *  handles it naturally. */
    virtual SValuePtr rotateRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted left. The return value will have the same width as operand @p a.  The @p nbits is interpreted as
     *  unsigned. New bits shifted into the value are zero. If @p nbits is equal to or larger than the width of @p a then the
     *  result is zero. */
    virtual SValuePtr shiftLeft(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right logically (no sign bit). The return value will have the same width as operand @p a. The @p
     *  nbits is interpreted as unsigned. New bits shifted into the value are zero. If  @p nbits is equal to or larger than the
     *  width of @p a then the result is zero. */
    virtual SValuePtr shiftRight(const SValuePtr &a, const SValuePtr &nbits) = 0;

    /** Returns arg shifted right arithmetically (with sign bit). The return value will have the same width as operand @p
     *  a. The @p nbits is interpreted as unsigned. New bits shifted into the value are the same as the most significant bit
     *  (the "sign bit"). If @p nbits is equal to or larger than the width of @p a then the result has all bits cleared or all
     *  bits set depending on whether the most significant bit was originally clear or set. */
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

    /** Equality comparison.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b holds. Both operands must be the same
     *  width. It doesn't matter if they are interpreted as signed or unsigned quantities.
     *
     * @{ */
    SValuePtr equal(const SValuePtr &a, const SValuePtr &b) ROSE_DEPRECATED("use isEqual instead");
    SValuePtr isEqual(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isNotEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    /** Comparison for unsigned values.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b is true when @p a and @p b are interpreted
     *  as unsigned values.  Both values must have the same width.  This operation is a convenience wrapper around other RISC
     *  operators.
     *
     * @{ */
    SValuePtr isUnsignedLessThan(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isUnsignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isUnsignedGreaterThan(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isUnsignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    /** Comparison for signed values.
     *
     *  Returns a Boolean to indicate whether the relationship between @p a and @p b is true when @p a and @p b are interpreted
     *  as signed values.  Both values must have the same width.  This operation is a convenience wrapper around other RISC
     *  operators.
     *
     * @{ */
    SValuePtr isSignedLessThan(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isSignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isSignedGreaterThan(const SValuePtr &a, const SValuePtr &b);
    SValuePtr isSignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Integer Arithmetic Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Extend (or shrink) operand @p a so it is @p nbits wide by adding or removing high-order bits. Added bits are always
     *  zeros. The result will be the specified @p new_width. */
    virtual SValuePtr unsignedExtend(const SValuePtr &a, size_t new_width) {
        return a->copy(new_width);
    }

    /** Sign extends a value. The result will the the specified @p new_width, which must be at least as large as the original
     * width. */
    virtual SValuePtr signExtend(const SValuePtr &a, size_t new_width) = 0;

    /** Adds two integers of equal size.  The width of @p a and @p b must be equal; the return value will have the same width
     * as @p a and @p b. */
    virtual SValuePtr add(const SValuePtr &a, const SValuePtr &b) = 0;

    /** Subtract one value from another.  This is not a virtual function because it can be implemented in terms of @ref add and
     * @ref negate. We define it because it's something that occurs often enough to warrant its own function. */
    SValuePtr subtract(const SValuePtr &minuend, const SValuePtr &subtrahend);

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
    //                                  Interrupt and system calls
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked for instructions that cause an interrupt.  The major and minor numbers are architecture specific.  For
     *  instance, an x86 INT instruction uses major number zero and the minor number is the interrupt number (e.g., 0x80 for
     *  Linux system calls), while an x86 SYSENTER instruction uses major number one. The minr operand for INT3 is -3 to
     *  distinguish it from the one-argument "INT 3" instruction which has slightly different semantics. */
    virtual void interrupt(int majr, int minr) {}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  State Accessing Operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Reads a value from a register.
     *
     *  The base implementation simply delegates to the current semantic State, which probably delegates to a register state,
     *  but subclasses are welcome to override this behavior at any level.
     *
     *  A register state will typically implement storage for hardware registers, but higher layers (the State, RiscOperators,
     *  Dispatcher, ...)  should not be concerned about the size of the register they're trying to read.  For example, a
     *  register state for a 32-bit x86 architecture will likely have a storage location for the 32-bit EAX register, but it
     *  should be possible to ask RiscOperators::readRegister to return the value of AX (the low-order 16-bits).  In order to
     *  accomplish this, some level of the readRegister delegations needs to invoke RiscOperators::extract() to obtain the low
     *  16 bits.  The RiscOperators object is passed along the delegation path for this purpose.  The inverse concat()
     *  operation will be needed at some level when we ask readRegister() to return a value that comes from multiple storage
     *  locations in the register state (such as can happen if an x86 register state holds individual status flags and we ask
     *  for the 32-bit EFLAGS register).
     *
     *  There needs to be a certain level of cooperation between the RiscOperators, State, and register state classes to decide
     *  which layer should invoke the extract() or concat() (or whatever other RISC operations might be necessary).
     */ 
    virtual SValuePtr readRegister(const RegisterDescriptor &reg) {
        ASSERT_not_null(state);
        return state->readRegister(reg, this);
    }

    /** Writes a value to a register.
     *
     *  The base implementation simply delegates to the current semantic State, which probably delegates to a register state,
     *  but subclasses are welcome to override this behavior at any level.
     *
     *  As with readRegister(), writeRegister() may need to perform various RISC operations in order to accomplish the task of
     *  writing a value to the specified register when the underlying register state doesn't actually store a value for that
     *  specific register. The RiscOperations object is passed along for that purpose.  See readRegister() for more details. */
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &a) {
        ASSERT_not_null(state);
        state->writeRegister(reg, a, this);
    }

    /** Reads a value from memory.
     *
     *  The implementation (in subclasses) will typically delegate much of the work to State::readMemory().
     *
     *  A MemoryState will implement storage for memory locations and might impose certain restrictions, such as "all memory
     *  values must be eight bits".  However, the RiscOperators::readMemory() should not have these constraints so that it can
     *  be called from a variety of Dispatcher subclass (e.g., the DispatcherX86 class assumes that RiscOperators::readMemory()
     *  is capable of reading 32-bit values from little-endian memory). The designers of the MemoryState, State, and
     *  RiscOperators should collaborate to decide which layer (RiscOperators, State, or MemoryState) is reponsible for
     *  combining individual memory locations into larger values.  A RiscOperators object is passed along the chain of
     *  delegations for this purpose. The RiscOperators might also contain other data that's import during the process, such as
     *  an SMT solver.
     *
     *  The @p segreg argument is an optional segment register. Most architectures have a flat virtual address space and will
     *  pass a default-constructed register descriptor whose is_valid() method returns false.
     *
     *  The @p cond argument is a Boolean value that indicates whether this is a true read operation. If @p cond can be proven
     *  to be false then the read is a no-op and returns an arbitrary value.
     *
     *  The @p dflt argument determines the size of the value to be read. This argument is also passed along to the lower
     *  layers so that they can, if they desire, use it to initialize memory that has never been read or written before. */
    virtual SValuePtr readMemory(const RegisterDescriptor &segreg, const SValuePtr &addr, const SValuePtr &dflt,
                                 const SValuePtr &cond) = 0;

    /** Writes a value to memory.
     *
     *  The implementation (in subclasses) will typically delegate much of the work to State::readMemory().  See readMemory()
     *  for more information.
     *
     *  The @p segreg argument is an optional segment register. Most architectures have a flat virtual address space and will
     *  pass a default-constructed register descriptor whose is_valid() method returns false.
     *
     *  The @p cond argument is a Boolean value that indicates whether this is a true write operation. If @p cond can be proved
     *  to be false then writeMemory is a no-op. */
    virtual void writeMemory(const RegisterDescriptor &segreg, const SValuePtr &addr, const SValuePtr &data,
                             const SValuePtr &cond) = 0;
};

/*******************************************************************************************************************************
 *                                      Instruction Dispatcher
 *******************************************************************************************************************************/

/** Smart pointer to a Dispatcher object. Dispatcher objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

/** Functor that knows how to dispatch a single kind of instruction. */
class InsnProcessor {
public:
    virtual ~InsnProcessor() {}
    virtual void process(const DispatcherPtr &dispatcher, SgAsmInstruction *insn) = 0;
};
    
/** Dispatches instructions through the RISC layer.
 *
 *  The dispatcher is the instruction semantics entity that translates a high-level architecture-dependent instruction into a
 *  sequence of RISC operators whose interface is defined by ROSE. These classes are the key in ROSE's ability to connect a
 *  variety of instruction set architectures to a variety of semantic domains.
 *
 *  Each dispatcher contains a table indexed by the machine instruction "kind" (e.g., SgAsmMipsInstruction::get_kind()). The
 *  table stores functors derived from the abstract InsnProcessor class.  (FIXME: The functors are not currently reference
 *  counted; they are owned by the dispatcher and deleted when the dispatcher is destroyed. [Robb Matzke 2013-03-04])
 *
 *  Dispatcher objects are allocated on the heap and reference counted.  The BaseSemantics::Dispatcher is an abstract class
 *  that defines the interface.  See the rose::BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts
 *  fit together. */
class Dispatcher: public boost::enable_shared_from_this<Dispatcher> {
protected:
    RiscOperatorsPtr operators;
    const RegisterDictionary *regdict;                  /**< See set_register_dictionary(). */
    size_t addrWidth_;                                  /**< Width of memory addresses in bits. */
    bool autoResetInstructionPointer_;                  /**< Reset instruction pointer register for each instruction. */

    // Dispatchers keep a table of all the kinds of instructions they can handle.  The lookup key is typically some sort of
    // instruction identifier, such as from SgAsmX86Instruction::get_kind(), and comes from the iproc_key() virtual method.
    typedef std::vector<InsnProcessor*> InsnProcessors;
    InsnProcessors iproc_table;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    // Prototypical constructor
    Dispatcher(): regdict(NULL), addrWidth_(0), autoResetInstructionPointer_(true) {}

    // Prototypical constructor
    Dispatcher(size_t addrWidth, const RegisterDictionary *regs)
        : regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {}

    Dispatcher(const RiscOperatorsPtr &ops, size_t addrWidth, const RegisterDictionary *regs)
        : operators(ops), regdict(regs), addrWidth_(addrWidth), autoResetInstructionPointer_(true) {
        ASSERT_not_null(operators);
        ASSERT_not_null(regs);
    }

public:
    virtual ~Dispatcher() {
        for (InsnProcessors::iterator iter=iproc_table.begin(); iter!=iproc_table.end(); ++iter)
            delete *iter;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors. None since this is an abstract class


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Virtual constructor. */
    virtual DispatcherPtr create(const RiscOperatorsPtr &ops, size_t addrWidth=0, const RegisterDictionary *regs=NULL) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods to process instructions
public:
    /** Process a single instruction. */
    virtual void processInstruction(SgAsmInstruction *insn);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Instruction processor table operations
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

    /** Set an iproc table entry to the specified value.
     *
     *  The @p iproc object will become owned by this dispatcher and deleted when this dispatcher is destroyed. */
    virtual void iproc_set(int key, InsnProcessor *iproc);

    /** Obtain an iproc table entry for the specified key. */
    virtual InsnProcessor *iproc_get(int key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convenience methods that defer the call to some member object
public:
    /** Get a pointer to the RISC operators object. */
    virtual RiscOperatorsPtr get_operators() const { return operators; }

    /** Get a pointer to the state object. The state is stored in the RISC operators object, so this is just here for
     *  convenience. */
    virtual StatePtr get_state() const { return operators ? operators->get_state() : StatePtr(); }

    /** Return the prototypical value.  The prototypical value comes from the RISC operators object. */
    virtual SValuePtr get_protoval() const { return operators ? operators->get_protoval() : SValuePtr(); }

    /** Returns the instruction that is being processed. The instruction comes from the get_insn() method of the RISC operators
     *  object. */
    virtual SgAsmInstruction *get_insn() const { return operators ? operators->get_insn() : NULL; }

    /** Return a new undefined semantic value. */
    virtual SValuePtr undefined_(size_t nbits) const {
        ASSERT_not_null(operators);
        return operators->undefined_(nbits);
    }

    /** Return a semantic value representing a number. */
    virtual SValuePtr number_(size_t nbits, uint64_t number) const {
        ASSERT_not_null(operators);
        return operators->number_(nbits, number);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods related to registers
public:
    /** Access the register dictionary.  The register dictionary defines the set of registers over which the RISC operators may
     *  operate. This should be same registers (or superset thereof) whose values are stored in the machine state(s).
     *  This dictionary is used by the Dispatcher class to translate register names to register descriptors.  For instance, to
     *  read from the "eax" register, the dispatcher will look up "eax" in its register dictionary and then pass that
     *  descriptor to the readRegister() RISC operation.  Register descriptors are also stored in instructions when the
     *  instruction is disassembled, so the dispatcher should probably be using the same registers as the disassembler, or a
     *  superset thereof.
     *
     *  The register dictionary should not be changed after a dispatcher is instantiated because the dispatcher's constructor
     *  may query the dictionary and cache the resultant register descriptors.
     * @{ */
    virtual const RegisterDictionary *get_register_dictionary() const {
        return regdict;
    }
    virtual void set_register_dictionary(const RegisterDictionary *regdict) {
        this->regdict = regdict;
    }
    /** @} */

    /** Lookup a register by name.  This dispatcher's register dictionary is consulted and the specified register is located by
     *  name.  If a bit width is specified (@p nbits) then it must match the size of register that was found.  If a valid
     *  register cannot be found then either an exception is thrown or an invalid register is returned depending on whether
     *  @p allowMissing is false or true, respectively. */
    virtual const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0, bool allowMissing=false);

    /** Property: Width of memory addresses.
     *
     *  This property defines the width of memory addresses. All memory reads and writes (and any other defined memory
     *  operations) should pass address expressions that are this width.  The address width cannot be changed once it's set.
     *
     * @{ */
    size_t addressWidth() const { return addrWidth_; }
    void addressWidth(size_t nbits);
    /** @} */

    /** Returns the instruction pointer register. */
    virtual RegisterDescriptor instructionPointerRegister() const = 0;

    /** Property: Reset instruction pointer register for each instruction.
     *
     *  If this property is set, then each time an instruction is processed, the first thing that happens is that the
     *  instruction pointer register is reset to the concrete address of the instruction.
     *
     * @{ */
    bool autoResetInstructionPointer() const { return autoResetInstructionPointer_; }
    void autoResetInstructionPointer(bool b) { autoResetInstructionPointer_ = b; }
    /** @} */
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous methods that tend to be the same for most dispatchers
public:

    /** Update the instruction pointer register.
     *
     *  Causes the instruction pointer register to point to the address following the specified instruction.  Since every
     *  instruction has a concrete address, we could simply set the instruction pointer to that concrete address. However, some
     *  analyses depend on having an instruction pointer value that's built up by processing one instruction after
     *  another.  Therefore, if we can recognize the register state implementation and determine that the instruction pointer
     *  registers' value is already stored, we'll increment that value, which might result in a concrete value depending on the
     *  semantic domain. Otherwise we just explicitly assign a new concrete value to that register. */
    virtual void advanceInstructionPointer(SgAsmInstruction*);

    /** Returns a register descriptor for the segment part of a memory reference expression.  Many architectures don't use
     *  segment registers (they have a flat virtual address space), in which case the returned register descriptor's is_valid()
     *  method returns false. */
    virtual RegisterDescriptor segmentRegister(SgAsmMemoryReferenceExpression*);

    /** Increment all auto-increment registers in the expression.  This method traverses the expression and increments each
     *  the register of each register reference expression that has a positive adjustment value.  If the same register is
     *  encountered multiple times then it is incremented multiple times. */
    virtual void incrementRegisters(SgAsmExpression*);

    /** Decrement all auto-decrement registers in the expression.  This method traverses the expression and increments each
     *  the register of each register reference expression that has a negative adjustment value.  If the same register is
     *  encountered multiple times then it is decremented multiple times. */
    virtual void decrementRegisters(SgAsmExpression*);

    /** Returns a memory address by evaluating the address expression.  The address expression can be either a constant or an
     *  expression containing operators and constants.  If @p nbits is non-zero then the result is sign extended or truncated
     *  to the specified width, otherwise the returned SValue is the natural width of the expression. */
    virtual SValuePtr effectiveAddress(SgAsmExpression*, size_t nbits=0);

    /** Reads an R-value expression.  The expression can be a constant, register reference, or memory reference.  The width of
     *  the returned value is specified by the @p value_nbits argument, and if this argument is zero then the width of the
     *  expression type is used.  The width of the address passed to lower-level memory access functions is specified by @p
     *  addr_nbits.  If @p addr_nbits is zero then the natural width of the effective address is passed to lower level
     *  functions. */
    virtual SValuePtr read(SgAsmExpression*, size_t value_nbits=0, size_t addr_nbits=0);

    /** Writes to an L-value expression. The expression can be a register or memory reference.  The width of the address passed
     *  to lower-level memory access functions is specified by @p addr_nbits.  If @p addr_nbits is zero then the natural width
     *  of the effective address is passed to lower level functions. */
    virtual void write(SgAsmExpression*, const SValuePtr &value, size_t addr_nbits=0);
};

/*******************************************************************************************************************************
 *                                      Printing
 *******************************************************************************************************************************/

std::ostream& operator<<(std::ostream&, const Exception&);
std::ostream& operator<<(std::ostream&, const SValue&);
std::ostream& operator<<(std::ostream&, const SValue::WithFormatter&);
std::ostream& operator<<(std::ostream&, const MemoryCell&);
std::ostream& operator<<(std::ostream&, const MemoryCell::WithFormatter&);
std::ostream& operator<<(std::ostream&, const MemoryState&);
std::ostream& operator<<(std::ostream&, const MemoryState::WithFormatter&);
std::ostream& operator<<(std::ostream&, const RegisterState&);
std::ostream& operator<<(std::ostream&, const RegisterState::WithFormatter&);
std::ostream& operator<<(std::ostream&, const State&);
std::ostream& operator<<(std::ostream&, const State::WithFormatter&);
std::ostream& operator<<(std::ostream&, const RiscOperators&);
std::ostream& operator<<(std::ostream&, const RiscOperators::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
