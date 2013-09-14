#ifndef Rose_BaseSemantics2_H
#define Rose_BaseSemantics2_H

#include "Registers.h"
#include "FormatRestorer.h"
#include "SMTSolver.h"

#include <cassert>
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
 *      using namespace BinaryAnalysis::InstructionSemantics2;
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
 *  When writing a subclass the author should define the (normal) constructors from the base class and any others that are
 *  needed, and they should have "protected" visibility.  The object should also define static instance() constructors and
 *  virtual create() constructors that are consistent with its base class.  The instance() constructor will almost always
 *  create a new object by passing all its arguments to the normal constructor, and then give ownership of the object to a
 *  smart pointer which is returned.  The virtual constructors will almost always just call the static constructor with the
 *  same arguments.  Here's an example:
 *
 *  @code
 *      typedef boost::shared_ptr<class MyThing> MyThingPtr;
 *      class MyThing: public OtherThing { // subclass of BaseSemantics::Thing
 *      private:
 *          double value;
 *      protected:
 *          // the normal C++ constructors; same arguments as for OtherThing::OtherThing()
 *          explicit MyThing(size_t width): OtherThing(width) {}
 *          MyThing(size_t width, double v): OtherThing(width), value(v) {}
 *      public:
 *          // the static allocating constructors, all named "instance"
 *          static MyThingPtr instance(size_t width) {
 *              return MyThingPtr(new MyThing(width));
 *          }
 *          static MyThingPtr instance(size_t width, double v) {
 *              return MyThingPtr(new MyThing(width, v));
 *          }
 *          // the virtual allocating constructors
 *          virtual BaseSemantics::ThingPtr create(size_t width) const override {
 *              return instance(width);
 *          }
 *          virtual BaseSemantics::ThingPtr generate(size_t width, double v) const override {
 *              return instance(width, v);
 *          }
 *     };
 *  @endcode
 *
 *  @section IS5 Other major changes
 *
 *  The new API exists in the BinaryAnalysis::InstructionSemantics2 name space and can coexist with the original API in
 *  BinaryAnalysis::InstructionSemantics&mdash;a program can use both APIs at the same time.
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
 *   using namespace BinaryAnalysis::InstructionSemantics2;
 *   BaseSemantics::RiscOperatorsPtr operators = SymbolicSemantics::RiscOperators::instance();
 *   BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(operators);
 *
 *   // Old API for comparison
 *   using namespace BinaryAnalysis::InstructionSemantics;
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
 *   using namespace BinaryAnalysis::InstructionSemantics2;
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
 *   using namespace BinaryAnalysis::InstructionSemantics;
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

/** Base classes for instruction semantics.  Basically, anything that is common to two or more instruction semantic
 *  domains will be factored out and placed in this name space. */
namespace BaseSemantics {

class RiscOperators;

/** Helper class for printing. Some semantic domains may want to pass some additional information to print methods on a
 *  per-call basis.  This base class provides something they can subclass to do that. A (optional) pointer to an instance of
 *  this class is passed to all semantic print() methods. */
class PrintHelper {
public:
    PrintHelper(): regdict(NULL) {}
    virtual ~PrintHelper() {}

    /** The register dictionary which is used for printing register names.
     * @{ */
    RegisterDictionary *get_register_dictionary() const { return regdict; }
    void set_register_dictionary(RegisterDictionary *rd) { regdict = rd; }
    /** @} */

protected:
    RegisterDictionary *regdict;
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
 *                                      Reference-counting pointers
 *******************************************************************************************************************************/

/** Referenc-counting pointer.  These pointers reference count the object to which they point and have an API similar to
 *  boost::shared_ptr<>.  However, this implementation is much faster (about 90% faster in tests) because it doesn't need to be
 *  as general-purpose as the Boost implementation.  This implementation doesn't support weak pointers or multi-threading, and
 *  it requires public access to an nrefs__ data member in the objects to which it points. */
template<class T>
class Pointer {
private:
    T *obj;     // object to which this pointer points; null for an empty pointer
public:
    typedef T element_type;

    /** Constructs an empty shared pointer. */
    Pointer(): obj(NULL) {}

    /** Constructs a shared pointer for an object.  If @p obj is non-null then its reference count is incremented. It is
     *  possible to create any number of shared pointers to the same object using this constructor. The expression "delete obj"
     *  must be well formed and must not invoke undefined behavior. */
    template<class Y>
    explicit Pointer(Y *obj): obj(obj) {
        if (obj!=NULL)
            ++obj->nrefs__;
    }

    /** Constructs a new pointer that shares ownership of the pointed-to object with the @p other pointer. The pointed-to
     *  object will only be deleted after both pointers are deleted.
     * @{ */
    Pointer(const Pointer &other): obj(other.obj) {
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL)
            ++obj->nrefs__;
    }
    template<class Y>
    Pointer(const Pointer<Y> &other): obj(other.get()) {
        if (obj!=NULL)
            ++obj->nrefs__;
    }
    /** @} */
    
    /** Conditionally deletes the pointed-to object.  The object is deleted when its reference count reaches zero. */
    ~Pointer() {
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL && 0==--obj->nrefs__)
            delete obj;
    }

    /** Assignment. This pointer is caused to point to the same object as @p other, decrementing the reference count for the
     * object originally pointed to by this pointer and incrementing the reference count for the object pointed by @p other.
     * @{ */
    Pointer& operator=(const Pointer &other) {
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL && 0==--obj->nrefs__)
            delete obj;
        obj = other.obj;
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL)
            ++obj->nrefs__;
        return *this;
    }
    template<class Y>
    Pointer& operator=(const Pointer<Y> &other) {
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL && 0==--obj->nrefs__)
            delete obj;
        obj = other.get();
        assert(obj==NULL || obj->nrefs__>0);
        if (obj!=NULL)
            ++obj->nrefs__;
        return *this;
    }
    /** @} */

    /** Reference to the pointed-to object.  An assertion will fail if assertions are enabled and this method is invoked on an
     *  empty pointer. */
    T& operator*() const {
        assert(obj!=NULL && obj->nrefs__>0);
        return *obj;
    }

    /** Dereference pointed-to object. The pointed-to object is returned. Returns null for empty pointers. */
    T* operator->() const {
        assert(!obj || obj->nrefs__>0);
        return obj; // may be null
    }

    /** Obtain the pointed-to object.  The pointed-to object is returned. Returns null for empty pointers. */
    T* get() const {
        assert(obj==NULL || obj->nrefs__>0);
        return obj; // may be null
    }

    /** Returns the pointed-to object's reference count. Returns zero for empty pointers. */
    long use_count() const {
        assert(obj==NULL || obj->nrefs__>0);
        return obj==NULL ? 0 : obj->nrefs__;
    }

    bool operator==(T *ptr) const { return obj==ptr; }
    bool operator!=(T *ptr) const { return obj!=ptr; }
    bool operator<(T *ptr) const { return obj<ptr; }
};

/** Cast one pointer type to another. This behaves the same as dynamic_cast<> except it updates the pointed-to object's
 *  reference count. */
template<class T, class U>
Pointer<T> dynamic_pointer_cast(const Pointer<U> &other)
{
    T* obj = dynamic_cast<T*>(other.get());
    return Pointer<T>(obj);
}

/*******************************************************************************************************************************
 *                                      Memory Allocators
 *******************************************************************************************************************************/

/** Fast memory allocator for small objects.  This memory allocator is used for semantic values and works by requesting large
 *  blocks of objects from the global operator new and maintaining a free list thereof.  User requests for objects return
 *  objects from the free list, and user deallocations return them to the free list.  The allocator will also be used for
 *  subclasses (unless the user overrides the operator new and operator delete in the subclass) and can handle a variety
 *  of object sizes. */
class Allocator {
private:
    struct Bucket {
        enum { SIZE = 81920 };          // FIXME: tune this
        char buffer[SIZE];
    };

    struct FreeItem {
        FreeItem *next;
    };

    enum { SIZE_DIVISOR = 8 };          // must be >= sizeof(FreeItem)
    enum { N_FREE_LISTS = 16 };         // number of lists. list[N] has objects of size <= (N+1)*SIZE_DIVISOR
    FreeItem *freelist[N_FREE_LISTS];

    // Fills the specified freelist by adding another Bucket-worth of objects.
    void fill_freelist(const int listn) { // hot
#if 0 /*DEBUGGING [Robb Matzke 2013-03-04]*/
        std::cerr <<"Allocator::fill_freelist(" <<listn <<")\n";
#endif
        assert(listn>=0 && listn<N_FREE_LISTS);
        const size_t object_size = (listn+1) * SIZE_DIVISOR;
        assert(object_size >= sizeof(FreeItem));
        Bucket *b = new Bucket;
        for (size_t offset=0; offset+object_size<Bucket::SIZE; offset+=object_size) {
            FreeItem *item = (FreeItem*)(b->buffer+offset);
            item->next = freelist[listn];
            freelist[listn] = item;
        }
        assert(freelist[listn]!=NULL);
    }

public:
    /** Allocate one object of specified size. The size must be non-zero. If the size is greater than the largest objects this
     *  class manages, then it will call the global operator new to satisfy the request (a warning is printed the first time
     *  this happens). */
    void *allocate(size_t size) { // hot
        assert(size>0);
        const int listn = (size-1) / SIZE_DIVISOR;
        if (listn>=N_FREE_LISTS) {
            static bool warned = false;
            if (!warned) {
                std::cerr <<"BinaryAnalysis::InstructionSemantics2::BaseSemantics::Allocator::allocate(): warning:"
                          <<" object is too large for allocator (" <<size <<" bytes); falling back to global allocator\n";
                warned = true;
            }
            return ::operator new(size);
        }
        if (NULL==freelist[listn])
            fill_freelist(listn);
        void *retval = freelist[listn];
        freelist[listn] = freelist[listn]->next;
#if 0 /*DEBUGGING [Robb Matzke 2013-03-04]*/
        std::cerr <<"Allocator::allocate(" <<size <<") = " <<retval <<"\n";
#endif
        return retval;
    }

    /** Free one object of specified size.  The @p size must be the same size that was used when the object was allocated. This
     *  is a no-op if @p ptr is null. */
    void deallocate(void *ptr, const size_t size) { // hot
#if 0 /*DEBUGGING [Robb Matzke 2013-03-04]*/
        std::cerr <<"Allocator::deallocate(" <<ptr <<", " <<size <<")\n";
#endif
        if (ptr) {
            assert(size>0);
            const int listn = (size-1) / SIZE_DIVISOR;
            if (listn>=N_FREE_LISTS)
                return ::operator delete(ptr);
            FreeItem *item = (FreeItem*)ptr;
            item->next = freelist[listn];
            freelist[listn] = item;
        }
    }
};

/*******************************************************************************************************************************
 *                                      Semantic Values
 *******************************************************************************************************************************/

/** Smart pointer to an SValue object. SValue objects are reference counted and should not be explicitly deleted.
 *
 *  Note: Although most semantic *Ptr types are based on boost::shared_ptr<>, SValuePtr uses a custom Pointer class which is
 *  substantially faster. */
typedef Pointer<class SValue> SValuePtr;

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
 *  that defines the interface.  See the BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit
 *  together.*/
class SValue {
public:
    long nrefs__; // shouldn't really be public, but need efficient reference from various Pointer<> classes
protected:
    size_t width;                               /** Width of the value in bits. Typically (not always) a power of two. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
    explicit SValue(size_t nbits): nrefs__(0), width(nbits) {}  // hot
    SValue(const SValue &other): nrefs__(0), width(other.width) {}

public:
    virtual ~SValue() { assert(0==nrefs__); } // hot

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating static constructor.  None are needed--this class is abstract.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating virtual constructors.  undefined_() needs underscores, so we do so consistently for all
    // these allocating virtual c'tors.  However, we use copy() rather than copy_() because this one is fundamentally
    // different: the object (this) is use for more than just selecting which virtual method to invoke.

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

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Custom allocation.
    static Allocator allocator;
    static void *operator new(size_t size) { return allocator.allocate(size); } // hot
    static void operator delete(void *ptr, size_t size) { allocator.deallocate(ptr, size); } // hot

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The rest of the API...

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

    /** Print a value to a stream. The value will normally occupy a single line and not contain leading space or line
     * termination. */
    virtual void print(std::ostream &output, PrintHelper *helper=NULL) const = 0;
};



/*******************************************************************************************************************************
 *                                      Register States
 *******************************************************************************************************************************/

/** Smart pointer to a RegisterState object.  RegisterState objects are reference counted and should not be explicitly
 *  deleted. */
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** The set of all registers and their values. RegisterState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::RegisterState is an abstract class that defines the interface.  See the
 *  BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.*/
class RegisterState {
protected:
    SValuePtr protoval;                         /**< Prototypical value for virtual constructors. */
    const RegisterDictionary *regdict;          /**< Registers that are able to be stored by this state. */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Normal, protected, C++ constructors
    explicit RegisterState(const SValuePtr &protoval, const RegisterDictionary *regdict): protoval(protoval), regdict(regdict) {
        assert(protoval!=NULL);
    }

public:
    virtual ~RegisterState() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors.  None are needed--this class is abstract.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Allocating virtual constructors.

    /** Virtual constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be used only
     *  to create additional instances of the value via its virtual constructors.  The prototypical value is normally of the
     *  same type for all parts of a semantic analysis. The register state must be compatible with the rest of the binary
     *  analysis objects in use. */
    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const = 0;

    /** Make a copy of this register state. */
    virtual RegisterStatePtr clone() const = 0;

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

    /** Set all registers to distinct undefined values. */
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
     *  The @p ph argument is an optional PrintHelper that is simply passed as the second argument of the
     *  underlying print methods for the register values. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const = 0;
};

/** A RegisterState for any architecture.
 *
 *  This state stores a list of non-overlapping registers and their values, typically only for the registers that have been
 *  accessed.  The state automatically switches between different representations when accessing a register that overlaps with
 *  one or more stored registers.  For instance, if the state stores 64-bit registers and the specimen suddently switches to
 *  32-bit mode, this state will split the 64-bit registers into 32-bit pieces.  If the analysis later returns to 64-bit mode,
 *  the 32-bit pieces are concatenated back to 64-bit values. This splitting and concatenation occurs on a per-register basis
 *  at the time the register is read or written. */
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
    typedef std::map<RegStore, RegPairs> Registers;

protected:
    bool init_to_zero;                          /**< Initialize registers to zero? */
    Registers registers;                        /**< Values for registers that have been accessed. */

protected:
    explicit RegisterStateGeneric(const SValuePtr &protoval, const RegisterDictionary *regdict)
        : RegisterState(protoval, regdict), init_to_zero(false) {
        clear();
    }

public:
    /** Static allocating constructor.  The @p protoval argument must be a non-null pointer to a semantic value which will be
     *  used only to create additional instances of the value via its virtual constructors.  The prototypical value is normally
     *  of the same type for all parts of a semantic analysis: its state and operator classes.
     *
     *  The register dictionary, @p regdict, describes the registers that can be stored by this register state, and should be
     *  compatible with the register dictionary used for other parts of binary analysis. */
    static RegisterStatePtr instance(const SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStatePtr(new RegisterStateGeneric(protoval, regdict));
    }

    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const /*override*/;
    virtual RegisterStatePtr clone() const /*override*/;

    virtual void clear() /*override*/;
    virtual void zero() /*override*/;
    virtual SValuePtr readRegister(const RegisterDescriptor &reg, RiscOperators *ops) /*override*/;
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops) /*override*/;
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const /*override*/;

protected:
    static void get_nonoverlapping_parts(const Extent &overlap, const RegPair &rp, RiscOperators *ops,
                                         RegPairs *pairs/*out*/);
};

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

    SValuePtr ip;                               /**< Instruction pointer. */
    SValuePtr gpr[n_gprs];                      /**< General-purpose registers */
    SValuePtr segreg[n_segregs];                /**< Segmentation registers. */
    SValuePtr flag[n_flags];                    /**< Control/status flags (i.e., FLAG register). */

protected:
    explicit RegisterStateX86(const SValuePtr &protoval, const RegisterDictionary *regdict): RegisterState(protoval, regdict) {
        clear();
    }

public:
    /** Static allocating constructor. The @p protoval argument must be a non-null pointer to a semantic value which will be
     *  used only to create additional instances of the value via its virtual constructors.  The prototypical value is normally
     *  of the same type for all parts of a semantic analysis: its state and operator classes. */
    static RegisterStatePtr instance(const SValuePtr &protoval, const RegisterDictionary *regdict) {
        return RegisterStatePtr(new RegisterStateX86(protoval, regdict));
    }

    virtual RegisterStatePtr create(const SValuePtr &protoval, const RegisterDictionary *regdict) const /*override*/;
    virtual RegisterStatePtr clone() const /*override*/;

    virtual void clear() /*override*/;
    virtual void zero() /* override*/;
    virtual SValuePtr readRegister(const RegisterDescriptor &reg, RiscOperators *ops) /*override*/;
    virtual void writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops) /*override*/;
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *ph=NULL) const /*override*/;

protected:
    // helpers for readRegister()
    virtual SValuePtr readRegisterGpr(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterFlag(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterSeg(const RegisterDescriptor &reg, RiscOperators *ops);
    virtual SValuePtr readRegisterIp(const RegisterDescriptor &reg, RiscOperators *ops);

    // helpers for writeRegister()
    virtual void writeRegisterGpr(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterFlag(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterSeg(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
    virtual void writeRegisterIp(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops);
};


/*******************************************************************************************************************************
 *                                      Memory State
 *******************************************************************************************************************************/

/** Smart pointer to a MemoryState object. MemoryState objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Represents all memory in the state. MemoryState objects are allocated on the heap and reference counted.  The
 *  BaseSemantics::MemoryState is an abstract class that defines the interface.  See the BinaryAnalysis::InstructionSemantics2
 *  namespace for an overview of how the parts fit together.*/
class MemoryState {
protected:
    SValuePtr protoval;                         /**< Prototypical value. */

    explicit MemoryState(const SValuePtr &protoval): protoval(protoval) {
        assert(protoval!=NULL);
    }

public:
    virtual ~MemoryState() {}

    /** Virtual allocating constructor.
     *
     *  Allocates and constructs a new MemoryState object having the same dynamic type as this object. A prototypical SValue
     *  must be supplied and will be used to construct any additional SValue objects needed during the operation of a
     *  MemoryState. */
    virtual MemoryStatePtr create(const SValuePtr &protoval) const = 0;

    /** Virtual allocating copy constructor. Creates a new MemoryState object which is a copy of this object. */
    virtual MemoryStatePtr clone() const = 0;

    /** Return the protoval.  The protoval is used to construct other values via its virtual constructors. */
    SValuePtr get_protoval() const { return protoval; }

    /** Clear memory. Removes all memory cells from this memory state. */
    virtual void clear() = 0;

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
     *  initialize a new memory location.  The manner in which the default is used depends on the implementation.
     *
     *  Footnote 1: A MemoryState::readMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::readMemory().  The designers of the MemoryState, State, and RiscOperators subclasses will need to
     *  coordinate to decide which layer should handle concatenating values from individual memory locations. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt, size_t nbits, RiscOperators *ops) = 0;

    /** Write a value to memory.
     *
     *  Consults the memory represented by this MemoryState object and possibly inserts the specified value.  The details of
     *  how a value is inserted into a memory state depends entirely on the implementation in a subclass and will probably be
     *  different for each semantic domain.
     *
     *  A MemoryState::writeMemory() call is the last in a sequence of delegations starting with
     *  RiscOperators::writeMemory(). The designers of the MemoryState, State, and RiscOperators will need to coordinate to
     *  decide which layer (if any) should handle splitting a multi-byte value into multiple memory locations. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *ops) = 0;

    /** Print a memory state to more than one line of output. */
    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *helper=NULL) const = 0;
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
class MemoryCell {
protected:
    SValuePtr address;                          /**< Address of memory cell. */
    SValuePtr value;                            /**< Value stored at that address. */

protected:
    // protected constructors
    MemoryCell(const SValuePtr &address, const SValuePtr &value)
        : address(address), value(value) {
        assert(address!=NULL);
        assert(value!=NULL);
    }

public:
    /** Static allocating constructor. Creates a new memory cell object with the specified address and value. */
    static MemoryCellPtr instance(const SValuePtr &address, const SValuePtr &value) {
        return MemoryCellPtr(new MemoryCell(address, value));
    }

    /** Virtual allocating constructor. Creates a new memory cell object with the specified address and value. */
    virtual MemoryCellPtr create(const SValuePtr &address, const SValuePtr &value) {
        return instance(address, value);
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

    /** Determines whether two memory cells can alias one another.  Two cells may alias one another if it is possible that
     *  their addresses cause them to overlap.  For cells containing one-byte values, aliasing may occur if their two addresses
     *  may be equal; multi-byte cells will need to check ranges of addresses. */
    virtual bool may_alias(const MemoryCellPtr &other, RiscOperators *ops) const;

    /** Determines whether two memory cells must alias one another.  Two cells must alias one another when it can be proven
     * that their addresses cause them to overlap.  For cells containing one-byte values, aliasing must occur unless their
     * addresses can be different; multi-byte cells will need to check ranges of addresses. */
    virtual bool must_alias(const MemoryCellPtr &other, RiscOperators *ops) const;
    
    /** Print the memory cell on a single line. */
    virtual void print(std::ostream &o, PrintHelper *helper=NULL) const {
        o <<"addr=";
        address->print(o, helper);
        o <<" value=";
        value->print(o, helper);
    }
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

    explicit MemoryCellList(const MemoryCellPtr &protocell, const SValuePtr &protoval)
        : MemoryState(protoval), protocell(protocell), byte_restricted(true) {
        assert(protocell!=NULL);
    }

public:
    /** Static allocating constructor.  This constructor uses the default type for the cell type (based on the semantic
     *  domain). */
    static MemoryCellListPtr instance(const SValuePtr &protoval) {
        MemoryCellPtr protocell = MemoryCell::instance(protoval, protoval);
        return MemoryCellListPtr(new MemoryCellList(protocell, protoval));
    }
    
    /** Static allocating constructor. */
    static MemoryCellListPtr instance(const MemoryCellPtr &protocell, const SValuePtr &protoval) {
        return MemoryCellListPtr(new MemoryCellList(protocell, protoval));
    }

    /** Virtual allocating constructor. This constructor uses the default type for the cell type (based on the semantic
     *  domain). */
    virtual MemoryStatePtr create(const SValuePtr &protoval) const /*override*/ {
        return instance(protoval);
    }
    
    /** Virtual allocating constructor. */
    virtual MemoryStatePtr create(const MemoryCellPtr &protocell, const SValuePtr &protoval) const {
        return instance(protocell, protoval);
    }

    /** Virtual allocating copy constructor. */
    virtual MemoryStatePtr clone() const /*override*/ {
        return MemoryStatePtr(new MemoryCellList(*this));               // FIXME?
    }

    /** Promote a base memory state pointer to a BaseSemantics::MemoryCellList pointer. The memory state @p m must have
     *  a BaseSemantics::MemoryCellList dynamic type. */
    static MemoryCellListPtr promote(const BaseSemantics::MemoryStatePtr &m) {
        MemoryCellListPtr retval = boost::dynamic_pointer_cast<MemoryCellList>(m);
        assert(retval!=NULL);
        return retval;
    }

    virtual void clear() /*override*/ {
        cells.clear();
    }

    /** Read a value from memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation scans the reverse chronological cell list until
     *  it finds a cell that must alias the specified addresses and value size. Along the way, it accumulates a list of cells
     *  that may alias the specified address.  If the accumulated list does not contain exactly one cell, or the scan fell off
     *  the end of the list, then @p dflt becomes the return value, otherwise the return value is the single value on the
     *  accumulated list. If the @p dflt value is returned, then it is also pushed onto the front of the cell list.
     *
     *  The base implementation assumes that all cells contain 8-bit values. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt, size_t nbits, RiscOperators *ops) /*override*/;

    /** Write a value to memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation creates a new memory cell and pushes it onto
     *  the front of the cell list.
     *
     *  The base implementation assumes that all cells contain 8-bit values. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *ops) /*override*/;

    virtual void print(std::ostream &o, const std::string prefix="", PrintHelper *helper=NULL) const /*override*/;

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
    virtual CellList scan(const BaseSemantics::SValuePtr &address, size_t nbits, RiscOperators *ops,
                          bool &short_circuited/*out*/) const;

    /** Returns the list of all memory cells.
     * @{ */
    virtual const CellList& get_cells() const { return cells; }
    virtual       CellList& get_cells()       { return cells; }
    /** @} */
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
 *  the interface.  See the BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit together.  */
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
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt, size_t nbits, RiscOperators *ops) {
        return memory->readMemory(address, dflt, nbits, ops);
    }

    /** Write a value to memory.
     *
     *  The BaseSemantics::writeMemory() implementation simply delegates to the memory state member of this state. See
     *  BaseSemantics::RiscOperators::writeMemory() for details. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *ops) {
        memory->writeMemory(addr, value, ops);
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
 *  that defines the interface.  See the BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit
 *  together. */
class RiscOperators {
protected:
    SValuePtr protoval;                         /**< Prototypical value used for its virtual constructors. */
    StatePtr state;                             /**< State upon which RISC operators operate. */
    SgAsmInstruction *cur_insn;                 /**< Current instruction, as set by latest startInstruction() call. */
    size_t ninsns;                              /**< Number of instructions processed. */
    SMTSolver *solver;                          /**< Optional SMT solver. */
    std::string name;                           /**< Name to use for debugging. */

protected:
    explicit RiscOperators(const SValuePtr &protoval, SMTSolver *solver=NULL)
        : protoval(protoval), cur_insn(NULL), ninsns(0), solver(solver) {
        assert(protoval!=NULL);
    }
    explicit RiscOperators(const StatePtr &state, SMTSolver *solver=NULL)
        : state(state), cur_insn(NULL), ninsns(0), solver(solver) {
        assert(state!=NULL);
        protoval = state->get_protoval();
    }
    /** @} */

public:
    virtual ~RiscOperators() {}

    // Static allocating constructor.  Each subclass should provide some static allocating constructors that will create
    // a new RiscOperators class. They should provide at least one version that will initialize the operators with default
    // prototypical values, etc.  These static allocating constructors are usually named "instance":
    //     static RiscOperatorsPtr instance(....);

    /** Virtual allocating constructor.  The @p protoval is a prototypical semantic value that is used as a factory to create
     *  additional values as necessary via its virtual constructors.  The state upon which the RISC operations operate must be
     *  provided by a separate call to the set_state() method. An optional SMT solver may be specified (see set_solver()). */
    virtual RiscOperatorsPtr create(const SValuePtr &protoval, SMTSolver *solver=NULL) const = 0;

    /** Virtual allocating constructor.  The supplied @p state is that upon which the RISC operations operate and is also used
     *  to define the prototypical semantic value. Other states can be supplied by calling set_state(). The prototypical
     *  semantic value is used as a factory to create additional values as necessary via its virtual constructors. An optional
     *  SMT solver may be specified (see set_solver()). */
    virtual RiscOperatorsPtr create(const StatePtr &state, SMTSolver *solver=NULL) const = 0;

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
    virtual StatePtr get_state() { return state; }
    virtual void set_state(const StatePtr &s) { state = s; }
    /** @} */

    /** A name used for debugging.
     * @{ */
    virtual const std::string& get_name() const { return name; }
    virtual void set_name(const std::string &s) { name = s; }
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
        return a->copy(new_width);
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
    //                                  Interrupt and system calls
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Invoked for instructions that cause an interrupt.  The major and minor numbers are architecture specific.  For
     *  instance, an x86 INT instruction uses major number zero and the minor number is the interrupt number (e.g., 0x80 for
     *  Linux system calls), while an x86 SYSENTER instruction uses major number one. */
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
        assert(state!=NULL);
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
        assert(state!=NULL);
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
     *  to be false then the read is a no-op and returns an arbitrary value. */
    virtual SValuePtr readMemory(const RegisterDescriptor &segreg, const SValuePtr &addr, const SValuePtr &cond,
                                 size_t nbits) = 0;


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
 *  counted [Robb Matzke 2013-03-04])
 *
 *  Dispatcher objects are allocated on the heap and reference counted.  The BaseSemantics::Dispatcher is an abstract class
 *  that defines the interface.  See the BinaryAnalysis::InstructionSemantics2 namespace for an overview of how the parts fit
 *  together. */
class Dispatcher: public boost::enable_shared_from_this<Dispatcher> {
protected:
    RiscOperatorsPtr operators;
    const RegisterDictionary *regdict;          /**< See set_register_dictionary(). */

    explicit Dispatcher(const RiscOperatorsPtr &ops): operators(ops), regdict(NULL) {
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous methods that tend to be the same for most dispatchers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Returns a register descriptor for the segment part of a memory reference expression.  Many architectures don't use
     *  segment registers (they have a flat virtual address space), in which case the returned register descriptor's is_valid()
     *  method returns false. */
    virtual RegisterDescriptor segmentRegister(SgAsmMemoryReferenceExpression*);

    /** Returns a memory address by evaluating the address expression.  The address expression can be either a constant or an
     *  expression containing operators and constants.  If @p nbits is non-zero then the result is sign extended or truncated
     *  to the specified width, otherwise the returned SValue is the natural width of the expression. */
    virtual SValuePtr effectiveAddress(SgAsmExpression*, size_t nbits=0);

    /** Reads an R-value expression.  The expression can be a constant, register reference, or memory reference.  The width of
     *  the returned value is specified by the @p value_nbits argument.  The width of the address passed to lower-level memory
     *  access functions is specified by @p addr_nbits.  If @p addr_nbits is zero then the natural width of the effective
     *  address is passed to lower level functions. */
    virtual SValuePtr read(SgAsmExpression*, size_t value_nbits, size_t addr_nbits=32);

    /** Writes to an L-value expression. The expression can be a register or memory reference.  The width of the address passed
     *  to lower-level memory access functions is specified by @p addr_nbits.  If @p addr_nbits is zero then the natural width
     *  of the effective address is passed to lower level functions. */
    virtual void write(SgAsmExpression*, const SValuePtr &value, size_t addr_nbits=32);
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
