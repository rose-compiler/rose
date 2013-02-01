Proposed improvements for Binary Instruction Semantics
======================================================


== Introduction ==

The original design for instruction semantics consisted of an
architecture-dependent dispatcher class template
(X86InstructionSemantics<>) that describes which RISC operations need
to be performed for each x86 instruction.  The RISC operations were
implemented in a "semantic policy" and operated on a "value type" that
itself was a template class--the argument being the width of the value
in bits.  The policy also contained a machine state struct.

Over time, as various parts of the policy were further abstracted, we
found that we needed more template parameters.  In other words, the
fact that we choose the ValueType at compile time means that we also
need to choose the State at compile time and the Policy at compile
time.  In turn, things that depend on the X86InstructionSemantics also
need to make all their choices at compile time.

== Problems ==

Some problems we've discovered with this design:

1. Some ValueTypes (e.g., the symbolic expression trees defined by
   InsnSemanticsExpr) aren't templates--they make the value bit width
   a run-time parameter.  In order to get these to be used in the
   instruction semantics framework, they need to be shoehorned in with
   an adapter class template (e.g., SymbolicSemantics::ValueType). In
   fact, even the very first implemented policy (FindConstantsPolicy)
   needed to shoehorn its ValueType in this way.

2. Since the instruction semantics framework makes all its choices
   about ValueType, State, and Policy at compile time, then higher
   level layers built on top of instruction semantics (e.g., control
   flow graphs, call graphs, dominance, the binary simulator) also
   need to be templates if they want to provide the user with a way to
   make those choices.  One ends up with a proliferation of templates
   and template arguments up to the user level.  Users generally
   aren't comfortable using templates.

3. The interfaces between related classes (e.g.,
   X86InstructionSemantics and the various semantics policies) aren't
   documented by code.  A user that wants to write their own policy,
   for instance, can look at an existing policy but it's not clear
   what methods are required, or what constraints exist for the
   arguments of those methods (e.g., must they be const-qualified?).
   What we have now is a special NullSemantics policy that tries to
   document the interface, and lots of references to that
   documentation.

4. Compiler error messages for templates are painfully verbose.  Not
   only are the error messages themselves long, but a single error in
   a policy, for instance, might result in 200 separate error messages
   (at least with GCC). This makes development of new policies slow
   since it's only practical with these long error lists to fix one
   error at a time between recompiles.

5. Subclassing doesn't work well because template member functions
   can't be virtual.  A user can override a member function from the
   base class, but if other base class methods will continue to use
   the version defined in the base class.

6. Having all the class template and member function template
   definitions in header files makes compiling very slow.  Check out
   how long it takes to compile projects/simulator, which uses
   instruction semantics extensively.

7. The current X86InstructionSemantics class template uses a huge
   switch statement to handle individual kinds of x86 instructions. In
   order to override an instruction, the user needs to create a
   subclass that handles that instruction and delegates to the base
   class for the other instructions.  This design also makes it
   difficult to "mix-in" handlers for x86 instructions at run-time.

== Fixes ==

1. Make ValueType a plain class rather than class template

Any removal of templates needs to start at the bottom of the
hierarchy.  Thus, the various ValueType class templates are the first
to be considered.  It should be possible to remove the size_t
bit-width argument for all the ValueType templates and make them plain
classes.  Unfortunately, ValueType is the most-mentioned class
template in all of ROSE (mentioned nearly 2000 times).

2. Combine all ValueType classes into a single hierarchy

Currently, each semantic domain defines its own ValueType which is
unrelated to any other ValueType.  Thus, any data structure that uses
a ValueType currently needs to be a class template.

We could make all the ValueType inherit from a base ValueType, but
that means all RISC operators in all semantic domains need to change
so they pass values by either pointer or reference, and they allocate
return values in the heap.  It also means that value storage (e.g., in
semantic state objects) needs to be by pointer.

Incidentally, this moves us closer to having semantic values behave
more like Sage nodes, and an eventual goal could be that we can store
semantic states in a BinaryAST file.

Also, some of the various ValueType classes already happen to define
similar member functions, like is_known() and get_known_value() which
could be moved into the base class, or made cast operators for a new
Concrete domain.

3. Make semantic state classes plain classes rather than templates

Once ValueType are a class hierarchy with BaseSemantics::ValueType as
the base class, then all the variety of machine state class templates
can be rewritten as plain classes that store heap-allocated instances
of BaseSemantics::ValueType.  We would define BaseSemantics::State and
all other state classes would inherit from that (as it currently
stands, each domain's State is unrelated to any other).

Having a BaseSemantics::State would also give us a place to declare
the part of the API that's common to all states (like clear(),
clear_memory(), clear_registers(), etc).  It would also make it easier
to generalize the memory representation and the register
representation--something that only a couple domains do now because it
adds too much complexity to class templates.

4. Separate machine state from RISC operations

Currently, a semantic policy describes both the machine state and the
RISC operations.  These are two orthogonal parts that should each be
its own class.  The BaseSemantics::State should define an API for
obtaining the state's memory and registers, which in turn should
define the API for reading and writing memory and registers.  The
policy's RISC operators for reading and writing machine state should
take a state object as an argument and defer to that state for the
actual read/write; the policy would do any necessary pre/post
processing of the state's read/write operation.

We already have analysis that swap states in/out of an instruction
semantics object by assuming that the state is being passed by
reference into the policy that's attached to the semantics object.
This change would make that more formal since the state object would
be a member of only the dispatcher class (e.g.,
X86InstructionSemantics).

(I'm not sure #4 should be implemented. Machine state is an integral
part of RISC operations in some semantic domains; it is likely that
the RISC operations and the State type need to be tightly integrated
and developed concurrently in those domains.)

5. Make policy class templates plain classes.

Policies are currently class templates because the choice of State and
ValueType needs to be made at compile time.  A Policy defines the RISC
operations and is given as a template argument to the
X86InstructionSemantics dispatcher. However, once the State and
ValueType are class hierarchies, we could define policies in terms of
their abstract base classes.

Currently, certain policies depend on certain capabilities in their
State and ValueType template arguments.  These capabilities are not
always well documented, but at least they're mostly checked at compile
time (if a State lacks a necessary method then you get compile
errors).  Using the virtual methods approach, each set of capabilities
would be an interface (abstract class), and the policy's RISC
operations will be doing much dynamic casting.  So we'll lose some
compile-time checking and incur runtime expense.

The policy classes should also be renamed since a "policy" is generally
used with templates.  A better name might be "RiscOperators" since
point 4 would have removed everything but the implementations of the
RISC operations.

6. Rename X86InstructionSemantics

The term "semantics" is already too overloaded, and it's redundant
with the namespace BinaryAnalysis::InstructionSemantics.  A better
name for these architecture-specific classes that translate CPU
instructions into ROSE's RISC operations would be "dispatcher" or
"translator".  Thus, the new name, fully qualified, would be
BinaryAnalysis::InstructionSemantics::DispatcherX86 (or
X86Dispatcher).

7. Make the architecture-specific dispatchers plain classes

If ValueType, State, and Policy are plain class hierarchies, then
there's no reason that the dispatchers need to be template
classes. 

8. All BaseSemantics classes will be abstract

If a new method is added to any of the base APIs then users will get
easy-to-read compiler errors refering to unimplemented methods in
their subclasses. Furthermore, it will be one error per method rather
than the torrent of errors one typically gets from undefined methods
in class templates.

9. Move member function definitions into *.C files

Since these are no longer templates, they can be moved to *.C files
and therefore compiled into the ROSE library.  This should drastically
improve compile time for user code that makes extensive use of
instruction semantics (like projects/simulator).

10. Make instruction dispatchers table-driven

Replace the huge switch statement with a table and a host of small
handlers, one per instruction kind.  All instruction handlers derive
from an abstract base class.

We could also use virtual functions for the instructions, but then
we'd have to have virtual functions for every potential instruction
kind (ROSE implements only about 160 x86 instructions). It also makes
it more difficult to share code for multiple kinds of instructions
because we can't anticipate what additional arguments will need to be
supplied to the handler (e.g., the 36 conditional branch (Jcc) and
test (SETcc) instructions could be implemented with a single
handler). Designing the table to use functors will allows users to
easily pass whatever other parameters and/or state they need.

== Example ==

Assuming all the above changes were implemented, setting up one or
ROSE's predefined instruction semantics would change from something
like this:

   using namespace BinaryAnalysis::InstructionSemantics;
   typedef SymbolicSemantics::State<SymbolicSemantics::ValueType> State;
   typedef SymbolicSemantics::Policy<State, SymbolicSemantics::ValueType> Policy;
   typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;
   Policy policy;
   Semantics semantics(policy);

to something like this:

   using namespace BinaryAnalysis::InstructionSemantics;
   SymbolicSemantics::ValueType prototype_value;
   SymbolicSemantics::State state(prototype_value);
   SymbolicSemantics::RiscOperators operators;
   X86Dispatcher dispatcher(state, operators);

The prototype_value is used only for its virtual constructor.
Using a user-defined semantics would look similar.

== Example ==

A RISC operator would change from something like this:

   template <size_t BeginAt, size_t EndAt, size_t nBits>
   ValueType<EndAt-BeginAt> extract(const ValueType<nBits> &a) const {
      ValueType<EndAt-BeginAt> retval = ...
      ...
      return retval;
   }

to

   BaseSemantics::ValueType *
   extract(size_t beginAt, size_t endAt, const BaseSemantics::ValueType *a) {
      ValueType *retval = dynamic_cast<ValueType*>(a->create(endAt-beginAt));
      retval->....
      return retval;
   }


== Example ==

User-defined subclasses would not need to do anything with templates.
Therefore, subclassing SymbolicSemantics to redefine it's
unsignedMultiply method would change from:

    template <
	template <template <size_t> class ValueType> class State,
	template <size_t> class ValueType
	>
    class MyPolicy: public SymbolicSemantics::Policy<State, ValueType> {
    public:
	template <size_t nBitsA, size_t nBitsB>
	ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
	    ValueType<nBitsA+nBitsB> retval;
	    retval.set_state(retval.VAL_UNDEFINED);
	    return retval;
	}
    };

to this:

    class MyOperators: public SymbolicSemantics::Operators {
    public:
        ValueType *unsignedMultiply(const BaseSemantics::ValueType *a, const BaseSemantics::ValueType *b) override {
            ValueType *retval = dynamic_cast<ValueType>(a->create(a->get_width() + b->get_width()));
	    retval->set_state(retval.VAL_UNDEFINED);
	    return retval;
        }
    };

and furthermore, any base class that calls unsignedMultiply() will
actually call MyOperators::unsignedMultiply() since it's a virtual
member function in BaseSemantics::Operators (which doesn't work in the
template-based design).


== Experimental Results for Compile and Run Times ==

Using semanticSpeed2.C without distcc or ccache; best of three times;
with optimizations; tests run on passerina

Compiler timings were from (optimization, no-debug, no-profile)
$ rg-make clean && time env DISTCC_HOSTS= CCACHE_DISABLE=yes make ...

                         Using      Compile  Executable Run speed
Program                  templates? time (s) size (MB)  (insn/s)
------------------------ ---------- -------- ---------- ---------
nullSemantics1               yes       9.312      4.961        na
nullSemantics2                no       7.826      4.924        na
partialSymbolicSemantics1    yes      15.479      5.943 2,295,040
partialSymbolicSemantics2     no       7.623      4.864   234,989

Progress made for increasing performance of the non-template design by
running partialSymbolicSemantics for 60 seconds, measured in x86
instructions per second.  The specimen is the infinite loop:
	     _start: mov eax, 0
             loop:   add eax, 1
                     jmp loop

							Insn/s	 1M insns %chg
Baseline (initial implementation)			234,989	 4.256s
Caching of register descriptors				278,741  3.588s   15.7%
Custom implementation of boost::shared_ptr for SValue	450,311  2.221s   38.1%
Custom allocators for SValue classes	       		657,144  1.522s   31.5%

Original implementation (ultimate goal)		      2,295,040  0.4357
