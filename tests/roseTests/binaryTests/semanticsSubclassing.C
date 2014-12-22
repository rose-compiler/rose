// Demo how to subclass some instruction semantics stuff from SymbolicSemantics. This follows the procedure described in the
// doxygen documentation for the BinaryAnalysis::InstructionSemantics2::BaseSemantics namespace.

#include "rose.h"
#include "SymbolicSemantics2.h"

using namespace rose::BinaryAnalysis::InstructionSemantics2;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Smart pointer for MyMemoryState
typedef boost::shared_ptr<class MyMemoryState> MyMemoryStatePtr;

// MyState subclasses SymbolicSemantics::State
class MyMemoryState: public SymbolicSemantics::MemoryState {

    // Implement the same real c'tors as the super class. No need to document these since they're private and they would have
    // mostly the same documentation as the corresponding static allocating constructor.
protected:
    MyMemoryState(const BaseSemantics::MemoryCellPtr &protocell)
        : SymbolicSemantics::MemoryState(protocell) {}

    explicit MyMemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval)
        : SymbolicSemantics::MemoryState(addrProtoval, valProtoval) {}

    MyMemoryState(const MyMemoryState &other)
        : SymbolicSemantics::MemoryState(other) {}

    // Implement static allocating constructors in terms of real constructors; one for each real constructor. They're usually
    // named "instance".  We want doxygen comments for them since these are new methods, most of which can just refer to the
    // super class.
public:
    /** Instantiate a new memory state from ... */
    static MyMemoryStatePtr instance(const BaseSemantics::MemoryCellPtr &protocell) {
        return MyMemoryStatePtr(new MyMemoryState(protocell));
    }

    /** Instantiate a new memory state from ... */
    static MyMemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) {
        return MyMemoryStatePtr(new MyMemoryState(addrProtoval, valProtoval));
    }

    /** Instantiate a new memory state by deep-copying an existing state. */
    static MyMemoryStatePtr instance(const MyMemoryStatePtr &other) {
        return MyMemoryStatePtr(new MyMemoryState(*other));
    }

    // Implement virtual constructors for each of the real constructors.  These are normally named "create" (although SValue
    // uses other names).  The virtual copy constructor is named "clone" rather than "create" in order to distinguish the fact
    // that although "create" can be invoked on prototypical objects, "clone" needs to be invoked on a real object. It is also
    // to distinguish between a virtual default constructor that takes no arguments, and the virtual copy constructor which
    // also takes no arguments.  No need for doxygen comments since they're documented in the base class.
public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
#if 0
        // it's easier to call the real constructor than to cast the smart-pointer-to-const-object and promote, but if we
        // really want to do that, here's how:
        MyMemoryStatePtr self = promote(boost::const_pointer_cast<BaseSemantics::MemoryState>(shared_from_this()));
        return instance(self);
#else
        // the easy way
        return MyMemoryStatePtr(new MyMemoryState(*this));
#endif
    }

    // Implement the checking dynamic cast.  This should fail (e.g., assert) if the cast is invalid because it indicates a
    // program logic error rather than a usage error.  Boilerplate doxygen documentation.
public:
    /** Checked dynamic pointer cast. */
    static MyMemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        // For SValue types use BaseSemantics::dynamic_pointer_cast instead of boost::dynamic_pointer_cast
        MyMemoryStatePtr retval = boost::dynamic_pointer_cast<MyMemoryState>(x);
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
    // Real constructors
protected:
    explicit MyRiscOperators(const BaseSemantics::SValuePtr &protoval, rose::BinaryAnalysis::SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(protoval, solver) {}
    explicit MyRiscOperators(const BaseSemantics::StatePtr &state, rose::BinaryAnalysis::SMTSolver *solver=NULL)
        : SymbolicSemantics::RiscOperators(state, solver) {}

    // Static allocating constructors
public:
    static MyRiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return MyRiscOperatorsPtr(new MyRiscOperators(protoval, solver));
    }
    static MyRiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, rose::BinaryAnalysis::SMTSolver *solver=NULL) {
        return MyRiscOperatorsPtr(new MyRiscOperators(state, solver));
    }

    // Virtual constructors
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   rose::BinaryAnalysis::SMTSolver *solver=NULL) const {
        return instance(protoval, solver);
    }
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   rose::BinaryAnalysis::SMTSolver *solver=NULL) const {
        return instance(state, solver);
    }

    // Checked dynamic casts
public:
    static MyRiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        MyRiscOperatorsPtr retval = boost::dynamic_pointer_cast<MyRiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }

    // The rest of the class would normally override RISC operators and/or introduce new methods.

#if 0
    // See what happens if you don't use a dynamic constructor.
    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        // This is the wrong way to create a new value.  It assumes that the semantics lattice was built to use the
        // SymbolicSemantics::SValue, but in fact we might be using a subclass thereof.  The error won't be detected until
        // someone tries to use a SymbolicSemantics::SValue subtype, and they might not even notice until this type error
        // propagates to enough SValue instances.
        return SymbolicSemantics::SValue::number_(a->get_width(), 0);

        // This is the correct way. It invokes the virtual constructor from the prototypical value.
        return get_protoval()->number_(a->get_width(), 0);

        // This is also correct since all values are (or at least should be) the same type as the protoval.
        return a->number_(a->get_width(), 0);
    }
#endif



};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test that everything works like we expect.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestSemantics2.h"

int main()
{
    // Build the RiscOperators.  Since we're wanting to use our shiny new classes, we need to build the lattice manually.
    // Refer to SymbolicSemantics::RiscOperators::instance() to see what parts are required, and substitute our class names
    // where appropriate.
    rose::BinaryAnalysis::SMTSolver *solver = NULL;
    const RegisterDictionary *regdict = RegisterDictionary::dictionary_pentium4();
    BaseSemantics::SValuePtr protoval = SymbolicSemantics::SValue::instance();
    BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    BaseSemantics::MemoryStatePtr memory = MyMemoryState::instance(protoval, protoval);
    BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
    BaseSemantics::RiscOperatorsPtr ops = MyRiscOperators::instance(state, solver);

    // Build a tester.  We need to indicate exactly what types are being used in the RiscOperators (fortunately for us,
    // they're from the code above).
    TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                  MyMemoryStatePtr, BaseSemantics::StatePtr, MyRiscOperatorsPtr> tester;

    // Run the test. An exception is thrown if there's a problem.
    tester.test(ops);

    return 0;
}
