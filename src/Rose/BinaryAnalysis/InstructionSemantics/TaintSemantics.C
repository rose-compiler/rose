#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/TaintSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace TaintSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::~SValue() {}

SValue::SValue() {}

SValue::SValue(const size_t nbits)
    : Super(nbits) {}

SValue::SValue(const size_t nbits, const uint64_t number)
    : Super(nbits, number) {}

SValue::SValue(const ExprPtr expr)
    : Super(expr) {}

SValue::Ptr
SValue::instance() {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(1)));
}

SValue::Ptr
SValue::instance_bottom(const size_t nbits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits, "", ExprNode::BOTTOM)));
}

SValue::Ptr
SValue::instance_undefined(const size_t nbits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits)));
}

SValue::Ptr
SValue::instance_unspecified(const size_t nbits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nbits, "", ExprNode::UNSPECIFIED)));
}

SValue::Ptr
SValue::instance_integer(const size_t nbits, const uint64_t value) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerConstant(nbits, value)));
}

SValue::Ptr
SValue::instance_symbolic(const SymbolicExpression::Ptr &value) {
    ASSERT_not_null(value);
    return Ptr(new SValue(value));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(const size_t nbits) const {
    return instance_bottom(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(const size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(const size_t nbits) const {
    return instance_unspecified(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(const size_t nbits, const uint64_t value) const {
    return instance_integer(nbits, value);
}

BaseSemantics::SValue::Ptr
SValue::boolean_(const bool value) const {
    return instance_integer(1, value?1:0);
}

BaseSemantics::SValue::Ptr
SValue::copy(const size_t new_width) const {
    Ptr retval(new SValue(*this));
    if (new_width != 0 && new_width != retval->nBits())
        retval->set_width(new_width);
    return retval;
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &other_, const BaseSemantics::Merger::Ptr &merger_,
                            const SmtSolverPtr &solver) const {

    auto retval = Super::createOptionalMerge(other_, merger_, solver);

    SValue::Ptr other = promote(other_);
    const Taintedness taintedness = mergeTaintedness(this->taintedness(), other->taintedness());

    if (this->taintedness() != taintedness) {
        if (!retval)
            retval = copy();
        promote(*retval)->taintedness(taintedness);
    }

    return retval;
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) { // hot
    Ptr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter) const {
#if 0 // long format
    switch (taintedness_) {
        case Taintedness::BOTTOM:
            stream <<"taint{} ";
            break;
        case Taintedness::UNTAINTED:
            stream <<"taint{untainted} ";
            break;
        case Taintedness::TAINTED:
            stream <<"taint{tainted} ";
            break;
        case Taintedness::TOP:
            stream <<"taint{tainted,untainted} ";
            break;
    }
#else // short format
    switch (taintedness_) {
        case Taintedness::BOTTOM:
            break;
        case Taintedness::UNTAINTED:
            stream <<"untainted ";
            break;
        case Taintedness::TAINTED:
            stream <<"tainted ";
            break;
        case Taintedness::TOP:
            stream <<"tainted,untainted ";
            break;
    }
#endif
    Super::print(stream, formatter);
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert((unsigned)taintedness_);
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &other, const SmtSolverPtr &solver) const {
    // Taintedness is not part of the true value, but rather a property that says something about the true value. Therefore,
    // taintedness doesn't influence whether two values may be equal.
    return Super::may_equal(other, solver);
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other, const SmtSolverPtr &solver) const {
    // Taintedness is not part of the true value, but rather a property that says something about the true value. Therefore,
    // taintedness doesn't influence whether two values may be equal.
    return Super::must_equal(other, solver);
}

Taintedness
SValue::taintedness() const {
    return taintedness_;
}

void
SValue::taintedness(Taintedness t) {
    taintedness_ = t;
}

Taintedness
SValue::mergeTaintedness(Taintedness a, Taintedness b) {
    if (Taintedness::TOP == a || Taintedness::TOP == b) {
        return Taintedness::TOP;

    } else if (Taintedness::BOTTOM == a) {
        return b;

    } else if (Taintedness::BOTTOM == b) {
        return a;

    } else {
        ASSERT_require(Taintedness::TAINTED == a || Taintedness::UNTAINTED == a);
        ASSERT_require(Taintedness::TAINTED == b || Taintedness::UNTAINTED == b);
        if (a == b) {
            return a;
        } else {
            return Taintedness::TOP;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators() {}

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : Super(protoval, solver) {
    name("Taint");
    ASSERT_always_not_null(protoval);
    ASSERT_always_not_null2(as<SValue>(protoval),
                            "TaintSemantics supports only TaintSemantics::SValue types or derivatives thereof");
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : Super(state, solver) {
    name("Taint");
    ASSERT_always_not_null(state);
    ASSERT_always_not_null(state->registerState());
    ASSERT_always_not_null2(as<RegisterState>(state->registerState()),
                            "TaintSemantics supports only RegisterStateGeneric or derivatives thereof");
    ASSERT_always_not_null(state->protoval());
    ASSERT_always_not_null2(as<SValue>(state->protoval()),
                            "TaintSemantics supports only TaintSemantics::SValue types or derivatives thereof");
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict, const SmtSolver::Ptr &solver) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryListState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromProtoval(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(protoval, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromState(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(state, solver));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) const {
    return instanceFromProtoval(protoval, solver);
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) const {
    return instanceFromState(state, solver);
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

Taintedness
RiscOperators::mergeTaintedness(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return SValue::mergeTaintedness(a->taintedness(), b->taintedness());
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::and_(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::or_(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::xor_(a, b));

    if (!a->isConcrete() && !b->isConcrete() && a->must_equal(b)) {
        // This handles code like i386's "xor eax, eax". However, it's risky because if the operands come from different
        // locations then they might still be tainted.
        //
        // For example, this result should be untainted:
        //    xor eax, eax   ; untainted no matter the value of eax
        //
        // And this result should be untainted:
        //    mov ebx, eax
        //    xor eax, ebx   ; untainted no matter the original value of eax
        //
        // But this result should be tainted:
        //    mov ebx, eax
        //    shr ebx, 1
        //    and eax, 1
        //    and ebx, 1
        //    xor eax, ebx   ; taint depends on original eax value even if both bits are equal
        retval->taintedness(Taintedness::UNTAINTED);
    } else {
        retval->taintedness(mergeTaintedness(a, b));
    }
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr retval = SValue::promote(Super::invert(a_));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, size_t begin_bit, size_t end_bit) {
    SValue::Ptr retval = SValue::promote(Super::extract(a_, begin_bit, end_bit));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &lo_bits, const BaseSemantics::SValue::Ptr &hi_bits) {
    SValue::Ptr retval = SValue::promote(Super::concat(lo_bits, hi_bits));
    retval->taintedness(mergeTaintedness(lo_bits, hi_bits));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr retval = SValue::promote(Super::equalToZero(a_));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel_, const BaseSemantics::SValue::Ptr &a_,
                             const BaseSemantics::SValue::Ptr &b_, IteStatus &status)
{
    SValue::Ptr sel = SValue::promote(sel_);
    SValue::Ptr retval = SValue::promote(Super::iteWithStatus(sel_, a_, b_, status));

    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);

    switch (status) {
        case IteStatus::NEITHER:
            retval->taintedness(sel->taintedness());
            break;
        case IteStatus::A:
            retval->taintedness(mergeTaintedness(sel, a));
            break;
        case IteStatus::B:
            retval->taintedness(mergeTaintedness(sel, b));
            break;
        case IteStatus::BOTH:
            retval->taintedness(SValue::mergeTaintedness(sel->taintedness(),
                                                         SValue::mergeTaintedness(a->taintedness(), b->taintedness())));
            break;
    }

    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr retval = SValue::promote(Super::leastSignificantSetBit(a_));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr retval = SValue::promote(Super::mostSignificantSetBit(a_));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    SValue::Ptr retval = SValue::promote(Super::rotateLeft(a, sa));
    retval->taintedness(mergeTaintedness(a, sa));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    SValue::Ptr retval = SValue::promote(Super::rotateRight(a, sa));
    retval->taintedness(mergeTaintedness(a, sa));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    SValue::Ptr retval = SValue::promote(Super::shiftLeft(a, sa));
    retval->taintedness(mergeTaintedness(a, sa));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    SValue::Ptr retval = SValue::promote(Super::shiftRight(a, sa));
    retval->taintedness(mergeTaintedness(a, sa));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    SValue::Ptr retval = SValue::promote(Super::shiftRightArithmetic(a, sa));
    retval->taintedness(mergeTaintedness(a, sa));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a_, size_t newWidth) {
    SValue::Ptr retval = SValue::promote(Super::unsignedExtend(a_, newWidth));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::add(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                              const BaseSemantics::SValue::Ptr &c, BaseSemantics::SValue::Ptr &carryOut_/*out*/) {
    SValue::Ptr retval = SValue::promote(Super::addWithCarries(a, b, c, carryOut_));
    retval->taintedness(mergeTaintedness(a, b));
    retval->taintedness(mergeTaintedness(retval, c));
    SValue::Ptr carryOut = SValue::promote(carryOut_);
    carryOut->taintedness(retval->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_) {
    SValue::Ptr retval = SValue::promote(Super::negate(a_));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::signedDivide(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::signedModulo(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::signedMultiply(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::unsignedDivide(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::unsignedModulo(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = SValue::promote(Super::unsignedMultiply(a, b));
    retval->taintedness(mergeTaintedness(a, b));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t newWidth) {
    SValue::Ptr retval = SValue::promote(Super::signExtend(a_, newWidth));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpConvert(const BaseSemantics::SValue::Ptr &a_, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    SValue::Ptr retval = SValue::promote(Super::fpConvert(a_, aType, retType));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::reinterpret(const BaseSemantics::SValue::Ptr &a_, SgAsmType *retType) {
    SValue::Ptr retval = SValue::promote(Super::reinterpret(a_, retType));
    SValue::Ptr a = SValue::promote(a_);
    retval->taintedness(a->taintedness());
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::SValue);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::RiscOperators);
#endif

#endif
