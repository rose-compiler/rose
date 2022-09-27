// Demo how to subclass some instruction semantics stuff from SymbolicSemantics. This follows the procedure described in the
// doxygen documentation for the BinaryAnalysis::InstructionSemantics::BaseSemantics namespace.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/CommandLine.h>

using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Smart pointer for MyMemoryState
typedef boost::shared_ptr<class MyMemoryState> MyMemoryStatePtr;

// MyState subclasses SymbolicSemantics::State
class MyMemoryState: public SymbolicSemantics::MemoryState {
public:
    using Ptr = MyMemoryStatePtr;

    // Implement the same real c'tors as the super class. No need to document these since they're private and they would have
    // mostly the same documentation as the corresponding static allocating constructor.
protected:
    MyMemoryState(const BaseSemantics::MemoryCell::Ptr &protocell)
        : SymbolicSemantics::MemoryState(protocell) {}

    explicit MyMemoryState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval)
        : SymbolicSemantics::MemoryState(addrProtoval, valProtoval) {}

    MyMemoryState(const MyMemoryState &other)
        : SymbolicSemantics::MemoryState(other) {}

    // Implement static allocating constructors in terms of real constructors; one for each real constructor. They're usually
    // named "instance".  We want doxygen comments for them since these are new methods, most of which can just refer to the
    // super class.
public:
    /** Instantiate a new memory state from ... */
    static MyMemoryState::Ptr instance(const BaseSemantics::MemoryCell::Ptr &protocell) {
        return MyMemoryState::Ptr(new MyMemoryState(protocell));
    }

    /** Instantiate a new memory state from ... */
    static MyMemoryState::Ptr instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) {
        return MyMemoryState::Ptr(new MyMemoryState(addrProtoval, valProtoval));
    }

    /** Instantiate a new memory state by deep-copying an existing state. */
    static MyMemoryState::Ptr instance(const MyMemoryState::Ptr &other) {
        return MyMemoryState::Ptr(new MyMemoryState(*other));
    }

    // Implement virtual constructors for each of the real constructors.  These are normally named "create" (although SValue
    // uses other names).  The virtual copy constructor is named "clone" rather than "create" in order to distinguish the fact
    // that although "create" can be invoked on prototypical objects, "clone" needs to be invoked on a real object. It is also
    // to distinguish between a virtual default constructor that takes no arguments, and the virtual copy constructor which
    // also takes no arguments.  No need for doxygen comments since they're documented in the base class.
public:
    virtual BaseSemantics::MemoryState::Ptr create(const BaseSemantics::MemoryCell::Ptr &protocell) const override {
        return instance(protocell);
    }

    virtual BaseSemantics::MemoryState::Ptr create(const BaseSemantics::SValue::Ptr &addrProtoval,
                                                 const BaseSemantics::SValue::Ptr &valProtoval) const override {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryState::Ptr clone() const override {
#if 0
        // it's easier to call the real constructor than to cast the smart-pointer-to-const-object and promote, but if we
        // really want to do that, here's how:
        MyMemoryState::Ptr self = promote(boost::const_pointer_cast<BaseSemantics::MemoryState>(shared_from_this()));
        return instance(self);
#else
        // the easy way
        return MyMemoryState::Ptr(new MyMemoryState(*this));
#endif
    }

    // Implement the checking dynamic cast.  This should fail (e.g., assert) if the cast is invalid because it indicates a
    // program logic error rather than a usage error.  Boilerplate doxygen documentation.
public:
    /** Checked dynamic pointer cast. */
    static MyMemoryState::Ptr promote(const BaseSemantics::MemoryState::Ptr &x) {
        // For SValue types use BaseSemantics::dynamic_pointer_cast instead of boost::dynamic_pointer_cast
        MyMemoryState::Ptr retval = boost::dynamic_pointer_cast<MyMemoryState>(x);
        assert(retval!=NULL);
        return retval;
    }

    // The rest of this class is just our reimplementation of inherited methods that we need to override, and new methods
    // that we provide at this level of the class hierarchy...
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Subclassing RiscOperators is pretty much the same as subclassing State (except there's no deep-copy constructor).

typedef boost::shared_ptr<class MyRiscOperators> MyRiscOperatorsPtr;

class MyRiscOperators: public SymbolicSemantics::RiscOperators {
public:
    using Ptr = MyRiscOperatorsPtr;

    // Real constructors
protected:
    explicit MyRiscOperators(const BaseSemantics::SValue::Ptr &protoval,
                             const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr())
        : SymbolicSemantics::RiscOperators(protoval, solver) {}
    explicit MyRiscOperators(const BaseSemantics::State::Ptr &state,
                             const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr())
        : SymbolicSemantics::RiscOperators(state, solver) {}

    // Static allocating constructors
public:
    static MyRiscOperators::Ptr instance(const BaseSemantics::SValue::Ptr &protoval,
                                       const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return MyRiscOperators::Ptr(new MyRiscOperators(protoval, solver));
    }
    static MyRiscOperators::Ptr instance(const BaseSemantics::State::Ptr &state,
                                       const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) {
        return MyRiscOperators::Ptr(new MyRiscOperators(state, solver));
    }

    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperators::Ptr
    create(const BaseSemantics::SValue::Ptr &protoval,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const {
        return instance(protoval, solver);
    }
    virtual BaseSemantics::RiscOperators::Ptr
    create(const BaseSemantics::State::Ptr &state,
           const Rose::BinaryAnalysis::SmtSolverPtr &solver = Rose::BinaryAnalysis::SmtSolverPtr()) const {
        return instance(state, solver);
    }

    // Checked dynamic casts
public:
    static MyRiscOperators::Ptr promote(const BaseSemantics::RiscOperators::Ptr &x) {
        MyRiscOperators::Ptr retval = boost::dynamic_pointer_cast<MyRiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }

    // The rest of the class would normally override RISC operators and/or introduce new methods.

#if 0
    // See what happens if you don't use a dynamic constructor.
    virtual BaseSemantics::SValue::Ptr xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        // This is the wrong way to create a new value.  It assumes that the semantics lattice was built to use the
        // SymbolicSemantics::SValue, but in fact we might be using a subclass thereof.  The error won't be detected until
        // someone tries to use a SymbolicSemantics::SValue subtype, and they might not even notice until this type error
        // propagates to enough SValue instances.
        return SymbolicSemantics::SValue::number_(a->nBits(), 0);

        // This is the correct way. It invokes the virtual constructor from the prototypical value.
        return protoval()->number_(a->nBits(), 0);

        // This is also correct since all values are (or at least should be) the same type as the protoval.
        return a->number_(a->nBits(), 0);
    }
#endif



};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that everything works like we expect.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Rose/BinaryAnalysis/InstructionSemantics/TestSemantics.h>

int main()
{
    // Build the RiscOperators.  Since we're wanting to use our shiny new classes, we need to build the lattice manually.
    // Refer to SymbolicSemantics::RiscOperators::instance() to see what parts are required, and substitute our class names
    // where appropriate.
    Rose::BinaryAnalysis::SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
    RegisterDictionary::Ptr regdict = RegisterDictionary::instancePentium4();
    BaseSemantics::SValue::Ptr protoval = SymbolicSemantics::SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MyMemoryState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = BaseSemantics::State::instance(registers, memory);
    BaseSemantics::RiscOperators::Ptr ops = MyRiscOperators::instance(state, solver);

    // Build a tester.  We need to indicate exactly what types are being used in the RiscOperators (fortunately for us,
    // they're from the code above).
    TestSemantics<SymbolicSemantics::SValue::Ptr, BaseSemantics::RegisterStateGeneric::Ptr,
                  MyMemoryState::Ptr, BaseSemantics::State::Ptr, MyRiscOperators::Ptr> tester;

    // Run the test. An exception is thrown if there's a problem.
    tester.test(ops);

    return 0;
}

#endif
