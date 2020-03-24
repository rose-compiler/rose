#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {

/** Binary instruction semantics.
 *
 *  Entities in this namespace deal with the semantics of machine instructions, and with the process of "executing" a machine
 *  instruction in a particular semantic domain.  Instruction "execution" is a very broad term and can refer to execution in
 *  the tranditional sense where each instruction modifies the machine state (registers and memory) in a particular domain
 *  (concrete, interval, sign, symbolic, user-defined).  But it can also refer to any kind of analysis that depends on
 *  semantics of individual machine instructions (def-use, tainted-flow, etc).  It can even refer to the transformation of
 *  machine instructions in ROSE internal representation to some other representation (e.g., to ROSE RISC or LLVM assembly)
 *  where the other representation is built by "executing" the instruction.
 *
 * @section instruction_semantics_components Components of instruction semantics
 *
 *  ROSE's binary semantics framework has four major components: the dispatchers, RISC operators, states, and values.  Each
 *  component has a base class to define the interface and common functionality, and subclasses to provide implementation
 *  details.  A semantics framework is constructed at runtime by instantiating objects from these subclasses and connecting the
 *  objects together to form a lattice.
 *
 *  At the top of the lattice is a <em>dispatcher</em> (base class @ref BaseSemantics::Dispatcher "Dispatcher") that "executes"
 *  machine instructions by translating (or <em>lowering</em>) them to sequences of RISC-like operations. The subclasses of
 *  @ref BaseSemantics::Dispatcher "Dispatcher" implement various instruction set architectures (ISAs).
 *
 *  The dispatcher points to an object that defines the RISC-like operators. This object is instantiated from a subclass of
 *  @ref BaseSemantics::RiscOperators "RiscOperators", and defines the few dozen RISC-like operators in terms of modifications
 *  to a state, or collection of values.  Therefore, the @ref BaseSemantics::RiscOperators "RiscOperators" needs to point to
 *  the <em>current state</em>. Depending on the subclass, it might also point to a lazily-initialized <em>initial
 *  state</em>. It is common for an analysis to swap new states in and out of the @ref BaseSemantics::RiscOperators
 *  "RiscOperators" while the analysis runs.
 *
 *  The aforementioned states are objects instantiated from subclasses of @ref BaseSemantics::State "State", which points to at
 *  least two substate objects: a @ref BaseSemantics::MemoryState "MemoryState" that describes the values stored at memory
 *  addresses, and a @ref BaseSemantics::RegisterState "RegisterState" that describes the values stored in registers. Depending
 *  on the @ref BaseSemantics::State "State" subclass, a state may also contain additional data. The @ref
 *  BaseSemantics::MemoryState "MemoryState" and @ref BaseSemantics::RegisterState "RegisterState" are base classes, and their
 *  subclasses provide various mechanisms for storing the memory and registers. For instance, memory might be stored as a
 *  chronological list or a map, and registers might be stored as an array or map.
 *
 *  Up to this point, we haven't nailed down the definition of "value".  A semantic value is also an abstract concept whose
 *  interface is declared in the @ref BaseSemantics::SValue "SValue" base class, the subclasses of which define the details. A
 *  value could be a vector of bits (concrete); an interval defined by two concrete endpoints; a sign consisting of one of the
 *  values positive, negative, zero, top, or bottom; a symbolic expression composed of constants, variables, and operations; or
 *  pretty much anything you want as long as it implements the API defined in the @ref BaseSemantics::SValue "SValue" base
 *  class.  Many of the objects mentioned above need to be able to create new values, and therefore they point to a
 *  proto-typical value instance which forms the bottom of the lattice.
 *  
 *  Not all combinations of dispatcher, operators, states, and values are possible, although they are intended to be mostly
 *  interchangeable.  For instance, you could combine an x86 dispatcher with symbolic operators using chronological memory and
 *  generic register states and a symbolic value type, but it probably doesn't make sense to have all the same components but
 *  replacing the symbolic value type with a concrete value type.  To help keep things organized, collections of compatible
 *  types are placed in namespaces such as @ref SymbolicSemantics. These collections of compatible semantic types are called
 *  <em>semantic domains</em>. Mixing types between semantic domains sometimes works, depending on the domain.
 *
 * @section instruction_semantics_domains Major domains
 *
 *  As mentioned, a "semantic domain" is a collection of compatible semantic types contained in a namespace. ROSE provides a
 *  number of general-purpose domains but users are also expected to specialize these for specific purposes. Even within ROSE,
 *  many of the analyses specialize these general-purpose domains in order to do something more specific without needing to
 *  re-implement large portions of the infrastructure.
 *
 *  You can find the full list of general-purpose domains by looking for sub namespaces of this namespace whose
 *  names end with the word "Semantics".  Some important examples:
 *
 *  @li @ref ConcreteSemantics has values that are bit vectors. This domain would be a good base class if you were writing an
 *  emulator since emulators work only with specific known values (i.e., concrete values rather than intervals, symbolic
 *  expressions, etc.). An example of a concrete value is a 16-bit vector 1010101111001110.
 *
 *  @li @ref IntervalSemantics is a domain whose values are sets of non-contiguous ranges of values where each range is
 *  indicated by its first and last concrete value.  An example of a value in this domain is the set {[0000,0010], [1000,1011]}
 *  that represents the four-bit concrete values 0000, 0001, 0010, 1000, 1001, 1010, and 1011.
 *
 *  @li @ref SymbolicSemantics is a domain whose values are constants, variables, and operations that form an expression
 *  tree. Each vertex of the tree has a specific width measured in bits. Variable are named with the letter "v" followed by a
 *  decimal serial number as in "v1", "v2", etc.  The symbolic @ref SymbolicSemantics::SValue "SValue" points to the symbolic
 *  expression. Symbolic expression trees (which are immutable) are instances of @ref Rose::BinaryAnalysis::SymbolicExpr.
 *
 *  @section instruction_semantics_pointers Memory Management
 *
 *  Most of the instruction semantics objects are allocated on the heap and are reference counted. This is beneficial to the
 *  user because an analysis might create millions of objects and it would otherwise be a burden if the analysis author had to
 *  know when it was safe to delete objects.  It also allows an analysis to return results to a higher level and not worry
 *  about who now owns those objects.
 *
 *  There are two ways to allocate such objects: (1) you must know the name of the derived class from which to instantiate an
 *  object, or (2) you must have an instance of an object of the class you wish to instantiate.  The former method is used when
 *  you're constructing a semantics framework since that's the moment you know the names of the classes, and the latter method
 *  is used when the framework is running and the class names might not be known but an object is already available. The former
 *  method uses static member functions, and the latter method uses virtual member functions (C++ implementation of the OO term
 *  <em>virtual constructor</em>).
 *
 *  Additional information can be found under @ref instruction_semantics_specialization.
 *
 *  @section instruction_semantics_create Instantiating a Semantics Framework
 *
 *  Let's say you have an analysis that needs to process x86 instructions symbolically. The first thing you need to do is
 *  instantiate a semantics framework -- the lattice of objects that are instantiated from the particular semantics component
 *  subclasses.  You'll need a @ref BaseSemantics::DispatcherX86 "DispatcherX86" object to handle the instructions, which
 *  invokes the RISC-like operations defined by an instance of @ref SymbolicSemantics::RiscOperators, which can use a
 *  chronological memory state and a generic register state, and whose values are symbolic (@ref
 *  SymbolicSemantics::SValue). You'll need to tie all these objects together into a lattice with the disptatcher at the top.
 *  The constructors for the various components generally take arguments which are the lower layers of the lattice, therefore
 *  you'll need to build the lattice from the bottom up; that is, start by constructing a proto-typical value (i.e., a value
 *  from which new values can be created), then the register and memory states, which are then joined together into a single
 *  state object. Then create the RISC-like operations object and give it an initial state, and finally create the dispatcher
 *  that points to the RISC-like operations.
 *
 *  Most semantic domains (@ref SymbolicSemantics included) have a simplified @ref BaseSemantics::RiscOperators "RiscOperators"
 *  constructor that uses default types for some of the lower components, but we'll show the full monty here:
 *
 *  @code
 *      namespace Base = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
 *      namespace Symbolic = Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
 *      namespace P2 = Rose::BinaryAnalysis::Partitioner2;
 *
 *      P2::Partitioner partitioner = ....; // disassembly results
 *      SmtSolverPtr solver = ....; // optional SMT solver
 *
 *      const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
 *      Base::SValuePtr prototval = Symbolic::SValue::instance();
 *      Base::RegisterStatePtr regs = Symbolic::RegisterState::instance(protoval, regdict);
 *      Base::MemoryStatePtr mem = Symbolic::MemoryListState::instance(protoval, protoval);
 *      Base::StatePtr state = Symbolic::State::instance(regs, mem);
 *      Base::RiscOperatorsPtr ops = Symbolic::RiscOperators::instance(state, solver);
 *      Base::DispatcherPtr cpu = Base::DispatcherX86::instance(ops, 32, regdict);
 *  @endcode
 *
 *  @section instruction_semantics_specialization Specialization
 *
 *  The instruction semantics architecture is designed to allow users to specialize nearly every part of it, which is useful
 *  when creating an analysis that needs to override some small parts of the entire semantics framework. Lets say you need to
 *  write an analysis that uses a concrete domain (like a simulator) and you want it to report every memory address to which a
 *  value is written.  Such a domain would be identical in every respect to the ROSE-provided @ref ConcreteSemantics domain
 *  except the RISC-like operation for writing a value to memory needs to additionally print the address and value.
 *
 *  Since you're essentially creating a new domain derived from ROSE's concrete domain, you should create a namespace for your
 *  domain. Let's call it @c MySemantics.  Since the value type, memory state, register state, combined state are all the same,
 *  create typedefs within @c MySemantics that just alias the types in ROSE's concrete domain. Notice there's no alias for a
 *  dispatcher; this is because dispatchers are domain-agnostic--any dispatcher will work with any semantic domain.
 *
 *  The only class you need to change is the RiscOperators class in ROSE's concrete domain. Therefore, within your namespace,
 *  define a new class named @c MySemantics::RiscOperators that inherits from ROSE's @ref ConcreteSemantics::RiscOperators, and
 *  override the @c writeMemory method so it prints the address and value before delegating to the base class.  You'll also
 *  need to define three classes of constructors detailed in @ref instruction_semantics_constructors.
 *
 *  Finally, your analysis can instantiate a semantics framework using the components from the new @c MySemantics. The code to
 *  do this looks almost identical to the example instantiation we already saw, except the word @c Symbolic would be changed to
 *  @c MySemantics wherever it appears.
 *
 *  @section instruction_semantics_constructors Writing Constructors for Reference-counted Classes
 *
 *  Here are some additional details to help you implement subclasses of reference-counted classes: You should implement three
 *  versions of each constructor: the real C++ constructor, the static allocating constructor, and the virtual constructor.
 *  Fortunately, the amount of extra code needed is not substantial since the virtual constructor can call the static
 *  allocating constructor, which can call the real C++ constructor. You'll need to override each overload of the three
 *  versions of constructors from the base class. The three versions in more detail are:
 *
 *  @li <i>Real Constructors</i>: These are the normal C++ constructors. They should have protected access and are used
 *  only by authors of subclasses.  The other two versions of constructors described next are <em>constructors</em> in the
 *  sense of object-oriented languages in general, although in the C++ world they're not called "constructors".
 *
 *  @li <i>Static Allocating Constructors</i>: These are class methods (C++ static member functions) that allocate a specific
 *  kind of object on the heap and return a smart pointer to the object.  They are named "instance" to emphasize that they
 *  instantiate a new instance of a particular class and they return the pointer type that is specific to the class (i.e., not
 *  one of the BaseSemantics pointer types).  When an end user constructs a dispatcher, RISC operators, etc., they have
 *  particular classes in mind and use those classes' "instance" methods to create objects.  Static allocating constructors are
 *  seldom called by authors of subclasses; instead the author usually has an object whose provenance can be traced back to a
 *  user-created object (such as a prototypical object), and he invokes one of that object's virtual constructors.
 *
 *  @li <i>Virtual Constructors</i>: A virtual constructor creates a new object having the same run-time type as the object on
 *  which the method is invoked.  Virtual constructors are often named "create" with the virtual copy constructor named
 *  "copy" or "clone", however the SValue class hierarchy follows a different naming scheme for historic reason--its virtual
 *  constructors end with an underscore.  Virtual constructors return pointer types that defined in BaseSemantics. Subclass
 *  authors usually use this kind of constructor because it frees them from having to know a specific type and allows
 *  their classes to be easily subclassed. Virtual constructors are implemented in C++ as virtual member functions.
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
 */
namespace InstructionSemantics2 {

/** Diagnostics logging facility for instruction semantics. */
extern Sawyer::Message::Facility mlog;

/** Initialize diagnostics for instruction semantics. */
void initDiagnostics();

} // namespace
} // namespace
} // namespace

#endif
#endif
