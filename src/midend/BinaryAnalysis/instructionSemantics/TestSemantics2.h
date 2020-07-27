// Perform basic sanity checks on instruction semantics
#ifndef Rose_TestSemantics2_H
#define Rose_TestSemantics2_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "BaseSemantics2.h"
#include "CommandLine.h"

namespace Rose {
namespace BinaryAnalysis {              // documented elsewhere
namespace InstructionSemantics2 {       // documented elsewhere

/** Provides functions for testing binary instruction semantics.
 *
 *  Most instruction semantics errors occur in two situations:  an author of a subclass instantiates an incorrect SValue type,
 *  or a subclass fails to implement a virtual allocator (either due to the subclass author not implementing it, or due the the
 *  super class author adding new methods or changing method signatures).  This test harness attempts to catch most of those
 *  kinds of changes.
 *
 *  To use this test harness, instantiate a RiscOperators object and call the TestSemantics<>::test() method.  A
 *  BaseSemantics::Exception is thrown if an error is detected.
 *
 *  The template arguments for TestSemantics should be the smart pointer types of the classes used to build the RiscOperators
 *  that's passed to the test() method. */
template<class SValuePtr, class RegisterStatePtr, class MemoryStatePtr, class StatePtr, class RiscOperatorsPtr>
class TestSemantics {
public:
    typedef typename SValuePtr::Pointee SValue;
    typedef typename RegisterStatePtr::element_type RegisterState;
    typedef typename MemoryStatePtr::element_type MemoryState;
    typedef typename StatePtr::element_type State;
    typedef typename RiscOperatorsPtr::element_type RiscOperators;

    class Exception: public BaseSemantics::Exception {
    public:
        Exception(const std::string &mesg): BaseSemantics::Exception(mesg, NULL) {}
    };

    void require(bool assertion, const std::string &what_failed) {
        if (!assertion)
            throw Exception("failed assertion: "+what_failed);
    }

    template<typename Pointer>
    void nonnull(const Pointer &x, const std::string &what_failed) {
        if (x==NULL)
            throw Exception("must not be null: "+what_failed);
    }

    // check boost smart pointers
    template<class ToPtr, class FromPtr>
    void check_type(const FromPtr &x, const std::string &what_failed) {
        typedef typename ToPtr::element_type To;
        nonnull(x, what_failed);
        ToPtr y = boost::dynamic_pointer_cast<To>(x);
        if (y==NULL)
            throw Exception("wrong pointer type: "+what_failed);
    }

    // check SValue smart pointers
    void check_sval_type(const BaseSemantics::SValuePtr &x, const std::string &what_failed) {
        nonnull(x, what_failed);
        SValuePtr y = BaseSemantics::dynamic_pointer_cast<SValue>(x);
        if (y==NULL)
            throw Exception("wrong pointer type: "+what_failed);
    }
    
    // Compile-time checks for SValue
    class SValueSubclass: public SValue {
    public:
        explicit SValueSubclass(size_t nbits): SValue(nbits) {}
        SValueSubclass(const SValueSubclass &other): SValue(other) {}
    };

    // Compile-time checks for RegisterState
    class RegisterStateSubclass: public RegisterState {
    public:
        explicit RegisterStateSubclass(const SValuePtr &protoval, const RegisterDictionary *regdict)
            : RegisterState(protoval, regdict) {}
    };

    // Compile-time checks for MemoryState
    class MemoryStateSubclass: public MemoryState {
    public:
        explicit MemoryStateSubclass(const SValuePtr &protoval)
            : MemoryState(protoval) {}
    };

    // Compile-time checks for State
    class StateSubclass: public State {
    public:
        StateSubclass(const RegisterStatePtr &registers, const MemoryStatePtr &memory)
            : State(registers, memory) {}
        StateSubclass(const StateSubclass &other)
            : State(other) {}
    };

    // Compile-time checks for RiscOperators
    class RiscOperatorsSubclass: public RiscOperators {
    public:
        explicit RiscOperatorsSubclass(const SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
            : RiscOperators(protoval, solver) {}
        explicit RiscOperatorsSubclass(const StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr())
            : RiscOperators(state, solver) {}
    };

    // Run-time checks
    void test(const BaseSemantics::RiscOperatorsPtr &ops) {
        ByteOrder::Endianness savedByteOrder = ops->currentState()->memoryState()->get_byteOrder();
        ops->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_LSB);
        test(ops->protoval(), ops->currentState(), ops);
        ops->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_MSB);
        test(ops->protoval(), ops->currentState(), ops);
        ops->currentState()->memoryState()->set_byteOrder(savedByteOrder);
    }
    
    void test(const BaseSemantics::SValuePtr &protoval,
              const BaseSemantics::StatePtr &state,
              const BaseSemantics::RiscOperatorsPtr &ops) {

        const RegisterDictionary *regdict = RegisterDictionary::dictionary_pentium4();
        const RegisterDescriptor reg32 = regdict->findOrThrow("eip");
        const RegisterDescriptor segreg = regdict->findOrThrow("ss");
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // SValue
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SValuePtr v0;
        require(v0==NULL, "default SValue constructor");

        // Dynamic pointer casts
        check_sval_type(SValue::promote(protoval), "SValue::promote()");

        // Virtual constructor: undefined_()
        BaseSemantics::SValuePtr v1 = protoval->undefined_(8);
        check_sval_type(v1, "SValue::undefined_()");
        require(v1->get_width()==8, "SValue::undefined_() width");

        // Virtual constructor: unspecified_()
        BaseSemantics::SValuePtr v1b = protoval->unspecified_(8);
        check_sval_type(v1b, "SValue::unspecified_()");
        require(v1b->get_width()==8, "SValue::unspecified() width");

        // Virtual constructor: number_().  Note that we can't check that the number is actually concrete and has a value
        // because BaseSemantics defines only the API for is_number() and get_number() and not the semantics of those
        // methods. In fact, the NullSemantics domain doesn't make any distinction between concrete and abstract values--it
        // treats everything as abstract.
        BaseSemantics::SValuePtr v2 = protoval->number_(32, 123);
        check_sval_type(v2, "SValue::number_()");
        require(v2->get_width()==32, "SValue::number_() width");

        // Virtual constructor: boolean_()
        BaseSemantics::SValuePtr v3 = protoval->boolean_(true);
        check_sval_type(v3, "SValue::boolean_()");
        require(v3->get_width()==1, "SValue::boolean_() width");

        // Virtual constructor: copy()
        BaseSemantics::SValuePtr v4 = v3->copy();
        check_sval_type(v4, "SValue::copy()");
        require(v4!=v3, "SValue::copy() should have returned a new object");
        require(v4->get_width()==1, "SValue::copy() width");
        require(v4->is_number() == v3->is_number(), "copies should be identical");
        if (v4->is_number())
            require(v4->get_number() == v3->get_number(), "concrete copies should be identical");
        std::ostringstream v3str, v4str;
        v3str <<*v3;
        v4str <<*v4;
        require(v3str.str() == v4str.str(), "copies should be identical");

        // may_equal
        require(v3->may_equal(v3), "a value may_equal itself");
        require(v3->may_equal(v4), "a value may_equal a copy of itself");
        require(v4->may_equal(v3), "a value may_equal a copy of itself");

        // must_equal.  Note: must_equal(v3, v4) need not be true when v4 is a copy of v3, although most subclasses do this.
        require(v3->must_equal(v3), "a value must_equal itself");
        require(v3->must_equal(v4) == v4->must_equal(v3), "must_equal should be symmetric");
        

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // RegisterState (read/write is tested by RiscOperators)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Dynamic pointer cast
        BaseSemantics::RegisterStatePtr rs1 = state->registerState();
        check_type<RegisterStatePtr>(RegisterState::promote(rs1), "RegisterState::promote()");

        BaseSemantics::SValuePtr rs1v1 = rs1->protoval();
        check_sval_type(rs1v1, "RegisterState::protoval()");

        // Virtual constructors
        BaseSemantics::RegisterStatePtr rs3 = rs1->create(protoval, regdict);
        check_type<RegisterStatePtr>(rs3, "create()");
        require(rs3->get_register_dictionary()==regdict, "RegisterState::create() register dictionary");
        require(rs3 != rs1, "RegisterState::create() must return a new object");
        BaseSemantics::SValuePtr rs3v1 = rs3->protoval();
        check_sval_type(rs3v1, "RegisterState::protoval() after create()");

        BaseSemantics::RegisterStatePtr rs4 = rs1->clone();
        check_type<RegisterStatePtr>(rs4, "clone()");
        require(rs4 != rs1, "RegisterState::clone() must return a new object");
        require(rs4->get_register_dictionary()==rs1->get_register_dictionary(),
                "RegisterState::clone() must use the register dictionary from the source state");
        BaseSemantics::SValuePtr rs4v1 = rs4->protoval();
        check_sval_type(rs4v1, "RegisterState::protoval() after clone()");

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // MemoryState (read/write is tested by RiscOperators)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // Dynamic pointer cast
        BaseSemantics::MemoryStatePtr ms1 = state->memoryState();
        check_type<MemoryStatePtr>(MemoryState::promote(ms1), "MemoryState::promote()");

        BaseSemantics::SValuePtr ms1v1 = ms1->get_addr_protoval();
        check_sval_type(ms1v1, "MemoryState::get_addr_protoval()");

        BaseSemantics::SValuePtr ms1v2 = ms1->get_val_protoval();
        check_sval_type(ms1v2, "MemoryState::get_val_protoval()");

        // Virtual constructors
        BaseSemantics::MemoryStatePtr ms2 = ms1->create(protoval, protoval);
        require(ms2 != ms1, "MemoryState::create() must return a new state");
        check_type<MemoryStatePtr>(ms2, "MemoryState::create(protoval)");
        BaseSemantics::SValuePtr ms2v1 = ms2->get_addr_protoval();
        check_sval_type(ms2v1, "MemoryState::get_addr_protoval() after create");
        BaseSemantics::SValuePtr ms2v2 = ms2->get_val_protoval();
        check_sval_type(ms2v2, "MemoryState::get_val_protoval() after create");

        BaseSemantics::MemoryStatePtr ms3 = ms1->clone();
        require(ms3 != ms1, "MemoryState::clone must return a new state");
        check_type<MemoryStatePtr>(ms3, "MemoryState::clone()");
        BaseSemantics::SValuePtr ms3v1 = ms3->get_addr_protoval();
        check_sval_type(ms3v1, "MemoryState::get_addr_protoval() after clone");
        BaseSemantics::SValuePtr ms3v2 = ms3->get_val_protoval();
        check_sval_type(ms3v2, "MemoryState::get_val_protoval() after clone");
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // State (read/write is tested by RiscOperators)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Dynamic pointer casts
        check_type<StatePtr>(State::promote(state), "State::promote()");

        BaseSemantics::SValuePtr state_protoval = state->protoval();
        check_sval_type(state_protoval, "State::protoval()");

        // Virtual constructors
        BaseSemantics::StatePtr s1 = state->create(rs1, ms1);
        require(s1 != state, "State::create() must return a new state");
        check_type<StatePtr>(s1, "State::create(regs,mem)");
        require(s1->registerState()==rs1, "State::create() must use supplied register state");
        require(s1->memoryState()==ms1, "State::create() must use supplied memory state");

        BaseSemantics::StatePtr s2 = state->clone();
        require(s2 != state, "State::clone() must return a new state");
        check_type<StatePtr>(s2, "State::clone()");
        require(s2->registerState() != state->registerState(),
                "State::clone() must deep-copy the register state");
        require(s2->memoryState() != state->memoryState(),
                "State::clone() must deep-copy the memory state");

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // RiscOperators
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Dynamic pointer casts
        check_type<RiscOperatorsPtr>(RiscOperators::promote(ops), "RiscOperators::promote()");

        BaseSemantics::SValuePtr ops_protoval = ops->protoval();
        check_sval_type(ops_protoval, "RiscOperators::protoval()");
        
        // Virtual constructors
        BaseSemantics::RiscOperatorsPtr o1 = ops->create(protoval, solver);
        require(o1 != ops, "RiscOperators::create(protoval,solver) should return a new object");
        check_type<RiscOperatorsPtr>(o1, "RiscOperators::create(protoval,solver)");

        BaseSemantics::RiscOperatorsPtr o2 = ops->create(state, solver);
        require(o2 != ops, "RiscOperators::create(state,solver) should return a new object");
        check_type<RiscOperatorsPtr>(o2, "RiscOperators::create(state,solver)");
        
        BaseSemantics::StatePtr ops_orig_state = ops->currentState();
        check_type<StatePtr>(ops_orig_state, "RiscOperators::currentState()");

        // We shouldn't use the supplied state because these tests modify it.  So we'll make a copy of the state and use that,
        // and then restore the original state before we return (but leave our state there fore debugging if there's an
        // exception).  This has the side effect of implicitly checking that State::clone() works because if it didn't the
        // caller would see the mess we made here. State::clone was tested already.
        BaseSemantics::StatePtr our_state = ops_orig_state->clone();
        ops->currentState(our_state);
        require(ops->currentState() == our_state, "RiscOperators::currentState failed to change state");

        for (size_t i=0; i<4; ++i) {
            // Value-creating operators
            BaseSemantics::SValuePtr v32a, v32b, v8, v1;
            switch (i) {
                case 0:
                    v32a = ops->undefined_(32);
                    v32b = ops->undefined_(32);
                    v8 = ops->undefined_(8);
                    v1 = ops->undefined_(1);
                    break;
                case 1:
                    v32a = ops->undefined_(32);
                    v32b = ops->number_(32, 3);
                    v8 = ops->number_(8, 3);
                    v1 = ops->boolean_(false);
                    break;
                case 2:
                    v32a = ops->number_(32, 4);
                    v32b = ops->undefined_(32);
                    v8 = ops->undefined_(8);
                    v1 = ops->undefined_(1);
                    break;
                case 3:
                    v32a = ops->number_(32, 4);
                    v32b = ops->number_(32, 3);
                    v8 = ops->number_(8, 3);
                    v1 = ops->boolean_(true);
                    break;
            }
            check_sval_type(v32a, "RiscOperators value constructor");
            require(v32a->get_width()==32, "RiscOperators value constructor width");
            check_sval_type(v32b, "RiscOperators value constructor");
            require(v32b->get_width()==32, "RiscOperators value constructor width");
            check_sval_type(v8, "RiscOperators value constructor");
            require(v8->get_width()==8, "RiscOperators value constructor width");
            check_sval_type(v1, "RiscOperators value constructor");
            require(v1->get_width()==1, "RiscOperators value constructor width");

            // x86-specific operators
            BaseSemantics::SValuePtr ops_v4 = ops->filterCallTarget(v32a);
            check_sval_type(ops_v4, "RiscOperators::filterCallTarget");
            require(ops_v4->get_width()==32, "RiscOperators::filterCallTarget width");

            BaseSemantics::SValuePtr ops_v5 = ops->filterReturnTarget(v32a);
            check_sval_type(ops_v5, "RiscOperators::filterReturnTarget");
            require(ops_v5->get_width()==32, "RiscOperators::filterReturnTarget width");

            BaseSemantics::SValuePtr ops_v6 = ops->filterIndirectJumpTarget(v32a);
            check_sval_type(ops_v6, "RiscOperators::filterIndirectJumpTarget");
            require(ops_v6->get_width()==32, "RiscOperators::filterIndirectJumpTarget width");

            BaseSemantics::SValuePtr ops_v7 = ops->rdtsc();
            check_sval_type(ops_v7, "RiscOperators::rdtsc");
            require(ops_v7->get_width()==64, "RiscOperators::rdtsc width");

            BaseSemantics::SValuePtr ops_v8 = ops->and_(v32a, v32b);
            check_sval_type(ops_v8, "RiscOperators::and_");
            require(ops_v8->get_width()==32, "RiscOperators::and_ width");

            BaseSemantics::SValuePtr ops_v9 = ops->or_(v32a, v32b);
            check_sval_type(ops_v9, "RiscOperators::or_");
            require(ops_v9->get_width()==32, "RiscOperators::or_ width");

            BaseSemantics::SValuePtr ops_v10 = ops->xor_(v32a, v32b);
            check_sval_type(ops_v10, "RiscOperators::xor_");
            require(ops_v10->get_width()==32, "RiscOperators::xor_ width");

            BaseSemantics::SValuePtr ops_v11 = ops->invert(v32a);
            check_sval_type(ops_v11, "RiscOperators::invert");
            require(ops_v11->get_width()==32, "RiscOperators::invert width");

            BaseSemantics::SValuePtr ops_v12 = ops->extract(v32a, 5, 8);
            check_sval_type(ops_v12, "RiscOperators::extract");
            require(ops_v12->get_width()==3, "RiscOperators::extract width");

            BaseSemantics::SValuePtr ops_v13 = ops->concat(v32a, v32b);
            check_sval_type(ops_v13, "RiscOperators::concat");
            require(ops_v13->get_width()==64, "RiscOperators::concat width");

            BaseSemantics::SValuePtr ops_v14 = ops->leastSignificantSetBit(v32a);
            check_sval_type(ops_v14, "RiscOperators::leastSignificantSetBit");
            require(ops_v14->get_width()==32, "RiscOperators::leastSignificantSetBit width");

            BaseSemantics::SValuePtr ops_v15 = ops->mostSignificantSetBit(v32a);
            check_sval_type(ops_v15, "RiscOperators::mostSignificantSetBit");
            require(ops_v15->get_width()==32, "RiscOperators::mostSignificantSetBit width");

            BaseSemantics::SValuePtr ops_v16 = ops->rotateLeft(v32a, v8);
            check_sval_type(ops_v16, "RiscOperators::rotateLeft");
            require(ops_v16->get_width()==32, "RiscOperators::rotateLeft width");

            BaseSemantics::SValuePtr ops_v17 = ops->rotateRight(v32a, v8);
            check_sval_type(ops_v17, "RiscOperators::rotateRight");
            require(ops_v17->get_width()==32, "RiscOperators::rotateRight width");

            BaseSemantics::SValuePtr ops_v18 = ops->shiftLeft(v32a, v8);
            check_sval_type(ops_v18, "RiscOperators::shiftLeft");
            require(ops_v18->get_width()==32, "RiscOperators::shiftLeft width");

            BaseSemantics::SValuePtr ops_v19 = ops->shiftRight(v32a, v8);
            check_sval_type(ops_v19, "RiscOperators::shiftRight");
            require(ops_v19->get_width()==32, "RiscOperators::shiftRight width");

            BaseSemantics::SValuePtr ops_v20 = ops->shiftRightArithmetic(v32a, v8);
            check_sval_type(ops_v20, "RiscOperators::shiftRightArithmetic");
            require(ops_v20->get_width()==32, "RiscOperators::shiftRightArithmetic width");

            BaseSemantics::SValuePtr ops_v21 = ops->equalToZero(v32a);
            check_sval_type(ops_v21, "RiscOperators::equalToZero");
            require(ops_v21->get_width()==1, "RiscOperators::equalToZero width");

            BaseSemantics::SValuePtr ops_v22 = ops->ite(v1, v32a, v32b);
            check_sval_type(ops_v22, "RiscOperators::ite");
            require(ops_v22->get_width()==32, "RiscOperators::ite width");

            BaseSemantics::SValuePtr ops_v23 = ops->unsignedExtend(v8, 32);
            check_sval_type(ops_v23, "RiscOperators::unsignedExtend");
            require(ops_v23->get_width()==32, "RiscOperators::unsignedExtend width");

            BaseSemantics::SValuePtr ops_v24 = ops->unsignedExtend(v32a, 8);
            check_sval_type(ops_v24, "RiscOperators::unsignedExtend truncate");
            require(ops_v24->get_width()==8, "RiscOperators::unsignedExtend truncate width");

            BaseSemantics::SValuePtr ops_v25 = ops->signExtend(v8, 32);
            check_sval_type(ops_v25, "RiscOperators::signExtend");
            require(ops_v25->get_width()==32, "RiscOperators::signExtend width");

            BaseSemantics::SValuePtr ops_v26 = ops->add(v32a, v32b);
            check_sval_type(ops_v26, "RiscOperators::add");
            require(ops_v26->get_width()==32, "RiscOperators::add width");

            BaseSemantics::SValuePtr carry_out;
            BaseSemantics::SValuePtr ops_v27 = ops->addWithCarries(v32a, v32b, v1, carry_out);
            check_sval_type(ops_v27, "RiscOperators::addWithCarries");
            require(ops_v27->get_width()==32, "RiscOperators::addWithCarries width");
            check_sval_type(carry_out, "RiscOperators::addWithCarries carry_out");
            require(carry_out->get_width()==32, "RiscOperators::addWithCarries carry_out width");

            BaseSemantics::SValuePtr ops_v28 = ops->negate(v32a);
            check_sval_type(ops_v28, "RiscOperators::negate");
            require(ops_v28->get_width()==32, "RiscOperators::negate width");

            try {
                BaseSemantics::SValuePtr ops_v29 = ops->signedDivide(v32a, v8);
                check_sval_type(ops_v29, "RiscOperators::signedDivide");
                require(ops_v29->get_width()==32, "RiscOperators::signedDivide width");
            } catch (const BaseSemantics::Exception&) {
                // possible division by zero
            }

            try {
                BaseSemantics::SValuePtr ops_v30 = ops->signedModulo(v32a, v8);
                check_sval_type(ops_v30, "RiscOperators::signedModulo");
                require(ops_v30->get_width()==8, "RiscOperators::signedModulo width");
            } catch (const BaseSemantics::Exception&) {
                // possible division by zero
            }

            BaseSemantics::SValuePtr ops_v31 = ops->signedMultiply(v32a, v8);
            check_sval_type(ops_v31, "RiscOperators::signedMultiply");
            require(ops_v31->get_width()==40, "RiscOperators::signedMultiply width");

            try {
                BaseSemantics::SValuePtr ops_v32 = ops->unsignedDivide(v32a, v8);
                check_sval_type(ops_v32, "RiscOperators::unsignedDivide");
                require(ops_v32->get_width()==32, "RiscOperators::unsignedDivide width");
            } catch (const BaseSemantics::Exception&) {
                // possible division by zero
            }

            try {
                BaseSemantics::SValuePtr ops_v33 = ops->unsignedModulo(v32a, v8);
                check_sval_type(ops_v33, "RiscOperators::unsignedModulo");
                require(ops_v33->get_width()==8, "RiscOperators::unsignedModulo width");
            } catch (const BaseSemantics::Exception&) {
                // possible division by zero
            }

            BaseSemantics::SValuePtr ops_v34 = ops->unsignedMultiply(v32a, v8);
            check_sval_type(ops_v34, "RiscOperators::unsignedMultiply");
            require(ops_v34->get_width()==40, "RiscOperators::unsignedMultiply width");

            BaseSemantics::SValuePtr ops_v35 = ops->readRegister(reg32);
            check_sval_type(ops_v35, "RiscOperators::readRegister");
            require(ops_v35->get_width()==32, "RiscOperators::readRegister width");

            // We can't really check many semantics for readMemory because each MemoryState might behave differently.  For
            // example, we can't check that reading the same address twice in a row returns the same value both times because
            // the NullSemantics doesn't have this property.

            BaseSemantics::SValuePtr dflt8 = ops->number_(8, 0);
            BaseSemantics::SValuePtr ops_v36 = ops->readMemory(segreg, v32a, dflt8, v1);
            check_sval_type(ops_v36, "RiscOperators::readMemory byte");
            require(ops_v36->get_width()==8, "RiscOperators::readMemory byte width");

            BaseSemantics::SValuePtr dflt32 = ops->number_(32, 0);
            BaseSemantics::SValuePtr ops_v37 = ops->readMemory(segreg, v32a, dflt32, v1);
            check_sval_type(ops_v37, "RiscOperators::readMemory word");
            require(ops_v37->get_width()==32, "RiscOperators::readMemory word width");

            // Nothing to check for write memory other than that we can actually call it.  The problem is that writeMemory only
            // modifies a state and doesn't return anything we can test.  The specifics of how it modifies a memory state is
            // entirely up to the implementation, so we can't even test that writing a value to an address and then reading
            // from that address returns the value that was written (e.g., NullSemantics doesn't have this property).

            ops->writeMemory(segreg, v32a, dflt32, v1);

        }

        // Restore the original state
        ops->currentState(ops_orig_state);
    }
};
        
} // namespace
} // namespace
} // namespace

#endif
#endif
