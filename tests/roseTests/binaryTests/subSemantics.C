/* Tests whether an X86 instruction semantics policy can be subclassed. */

#include "rose.h"
#include "SymbolicSemantics.h"

using namespace BinaryAnalysis::InstructionSemantics;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstrate that we can subclass a the semantic value type.  Our new value type is the same as the value type used by ROSE's
// default symbolic domain, but we add a the ability for each value to have an enum constant that defines the state of the
// value.
template <size_t nBits>
class MyValueType: public SymbolicSemantics::ValueType<nBits> {
public:
    // Value state.  The VAL_UNDEFINED might be useful, but at this time ROSE's symbolic domain defines all RISC operations, so
    // there is never a case when some implemented x86 instruction produces an output value that depends on an unimplemented
    // RISC operation.  We'll force one of the RISC operations to generate this in our MyPolicy class for demonstration.
    enum State {
        VAL_INIT,                       // value is an initial value of some state, not the result of an x86 instruction
        VAL_UNDEFINED,                  // value is from an unimplemented RISC operation due to an x86 instruction
        VAL_DEFINED,                    // value is due to an x86 instruction, although it might be a free variable
    };

    // Reimplement the same constructors as for the super class
    MyValueType(std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(comment), state(VAL_DEFINED) {}
    MyValueType(const MyValueType &other)
        : SymbolicSemantics::ValueType<nBits>(other), state(VAL_DEFINED) {}
    explicit MyValueType(uint64_t n, std::string comment="")
        : SymbolicSemantics::ValueType<nBits>(n, comment), state(VAL_DEFINED) {}
    explicit MyValueType(const InsnSemanticsExpr::TreeNodePtr &node)
        : SymbolicSemantics::ValueType<nBits>(node), state(VAL_DEFINED) {}

    // Our own new methods to access the value state
    State get_state() const { return state; }
    void set_state(State  s) { state = s; }

protected:
    State state;
};




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstrate that we can subclass the machine state.  Our machine state is just like the state for ROSE's default symbolic
// domain, but we'll add some floating point registers.
template <template <size_t> class ValueType>
class MyState: public SymbolicSemantics::State<ValueType> {
public:
    enum { n_fpregs=8 };
    double fpregs[n_fpregs];
    size_t fptop;

    MyState(): fptop(0) {}

    // Augment some functions to operate also on our floating point registers.  There are a few others, but this is just a
    // demo.
    void zero_registers() {
        SymbolicSemantics::State<ValueType>::clear();
        memset(fpregs, 0, sizeof fpregs);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstrate that we can subclass the semantic policy.  This particular version requires that the State has floating point
// registers (needed for our readRegister()) and that the ValueType has value state members (needed for our
// unsignedMultiply()).
template <
    template <template <size_t> class ValueType> class State,
    template <size_t> class ValueType
    >
class MyPolicy: public SymbolicSemantics::Policy<State, ValueType> {
public:
    // Define c'tors same as superclass
    MyPolicy() {}
    MyPolicy(SMTSolver *solver): SymbolicSemantics::Policy<State, ValueType>(solver) {}

    // Augment the readRegister() RISC operator so it can read from floating point registers.
    template<size_t nBits>
    ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
        if (x86_regclass_st==reg.get_major() && reg.get_minor()<(unsigned)this->get_state().n_fpregs) {
#if 1 /*DEBUGGING [Robb Matzke 2012-12-21]*/
            std::cerr <<"MyPolicy::readRegister() register " <<reg.get_major() <<"." <<reg.get_minor() <<"\n";
#endif
            size_t regno = (reg.get_minor() + this->get_state().fptop) % this->get_state().n_fpregs;
            double value = this->get_state().fpregs[regno];
            // Since this is a simple demo, just cast the fp value to an integer of requested size.
            return ValueType<nBits>((uint64_t)value, "cast from double");
        }
        return SymbolicSemantics::Policy<State, ValueType>::template readRegister<nBits>(reg);
    }

    // Replace the unsignedMultiply() RISC operator so it always returns an undefined value (as if it were not implemented at
    // all).  This is subtly different than a RISC operator that returns a new, unbound symbolic variable (although none of
    // ROSE's symbolic-domain RISC operators do that).
    template <size_t nBitsA, size_t nBitsB>
    ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
        ValueType<nBitsA+nBitsB> retval;
        retval.set_state(retval.VAL_UNDEFINED);
#if 1 /*DEBUGGING [Robb Matzke 2012-12-21]*/
        std::cerr <<"MyPolicy::unsignedMultiply() (" <<a <<") * (" <<b <<") = " <<retval <<"\n";
#endif
        return retval;
    }
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main program parses the binary and then "runs" it by sending it through the instruction semantics.  This is a bit
// nonsensical since it "runs" the program not by following the control flow graph, but by following the abstract syntax
// tree. This keeps the demo small and easy to understand.

// This template is only here because I abhor cut-n-pasted code. It's not actually integral to demonstrating that ROSE's
// symbolic semantics domain can be subclassed--it's used by main() rather than creating a separate class for each test. Sorry
// about the nested template template arguments.  An example in main() uses an expanded version of this class template
// in case you're intimidated by this.
template <
    template <
        template <template <size_t> class> class,
        template <size_t> class
        > class Policy,
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
struct Test: AstSimpleProcessing {
    typedef X86InstructionSemantics<Policy<State, ValueType>, ValueType> Semantics;
    Policy<State, ValueType> policy;
    Semantics semantics;
    Test(): semantics(policy) {}
    virtual void visit(SgNode *node) /*override*/ {
        if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(node)) {
            policy.writeRegister("eip", ValueType<32>(insn->get_address()));
            semantics.processInstruction(insn);
        }
    }
};

int main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);

    // Test0 is the same as test1 but using an expanded Test<> so you can see what's going on if you're not comfortable with
    // templates.  This is more likely what user code will look like.
    std::cout <<"test0\n";
    struct Test0: AstSimpleProcessing {
        typedef SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> Policy;
        typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;
        Policy policy;
        Semantics semantics;
        Test0(): semantics(policy) {}
        virtual void visit(SgNode *node) /*override*/ {
            if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(node)) {
                policy.writeRegister("eip", SymbolicSemantics::ValueType<32>(insn->get_address()));
                semantics.processInstruction(insn);
            }
        }
    } test0;
    test0.traverse(project, preorder);

    // First example uses only ROSE's classes.
    std::cout <<"test1\n";
    Test<SymbolicSemantics::Policy, SymbolicSemantics::State, SymbolicSemantics::ValueType> test1;
    test1.traverse(project, preorder);

    // Second example uses our value type, but ROSE's state and policy. Since our value type's constructors always set the
    // state to VAL_DEFINED, that's all we'll ever see.  Of course the user can still set value states to something else. For
    // example, they might set all the values of the initial machine state to VAL_INIT.
    std::cout <<"test2\n";
    Test<SymbolicSemantics::Policy, SymbolicSemantics::State, MyValueType> test2;
    test2.traverse(project, preorder);

    // Third example uses our value type and state, but ROSE's policy.  The floating-point registers in our state will never
    // actually be used since ROSE's policy doesn't know about them, but we could still store things there explicitly.  The
    // value states will always be VAL_DEFINED as in the previous example.
    std::cout <<"test3\n";
    Test<SymbolicSemantics::Policy, MyState, MyValueType> test3;
    test3.traverse(project, preorder);

    // Fourth example uses our value type, state, and policy.  Our policy depends on features specific to our state an value
    // type, so it wouldn't be possible to use our policy but ROSE's state and/or value type.
    std::cout <<"test4\n";
    Test<MyPolicy, MyState, MyValueType> test4;
    test4.traverse(project, preorder);

    // Fifth example uses our state, but ROSE's value type and policy.  Our state doesn't depend on anything that's specific to
    // our value type, so we can plug in ROSE's value type.  This gives us the floating point registers although ROSE's policy
    // won't actually use them.
    std::cout <<"test5\n";
    Test<SymbolicSemantics::Policy, MyState, SymbolicSemantics::ValueType> test5;
    test5.traverse(project, preorder);

    return 0;
}
