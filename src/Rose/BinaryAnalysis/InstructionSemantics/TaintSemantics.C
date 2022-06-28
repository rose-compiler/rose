#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TaintSemantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace TaintSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValuePtr &other_, const BaseSemantics::MergerPtr &merger_,
                            const SmtSolverPtr &solver) const {

    auto retval = Super::createOptionalMerge(other_, merger_, solver);

    SValuePtr other = promote(other_);
    const Taintedness taintedness = mergeTaintedness(this->taintedness(), other->taintedness());

    if (this->taintedness() != taintedness) {
        if (!retval)
            retval = copy();
        promote(*retval)->taintedness(taintedness);
    }

    return retval;
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter) const {
#if 0 // long format
    switch (taintedness_) {
        case Taintedness::UNKNOWN:
            stream <<"TaintUnknown ";
            break;
        case Taintedness::NO:
            stream <<"Untainted ";
            break;
        case Taintedness::YES:
            stream <<"Tainted ";
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
SValue::may_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const {
    SValue::Ptr other = promote(other_);

    if (Taintedness::UNKNOWN == this->taintedness() || Taintedness::UNKNOWN == other->taintedness()) {
        return true;
    } else if (this->taintedness() != other->taintedness()) {
        return false;
    } else {
        return Super::may_equal(other_, solver);
    }
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const {
    SValue::Ptr other = promote(other_);

    if (Taintedness::UNKNOWN == this->taintedness() || Taintedness::UNKNOWN == other->taintedness()) {
        return false;
    } else if (this->taintedness() != other->taintedness()) {
        return false;
    } else {
        return Super::may_equal(other_, solver);
    }
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
    if (Taintedness::UNKNOWN == a || Taintedness::UNKNOWN == b) {
        return Taintedness::UNKNOWN;
    } else {
        return std::max(a, b);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    if (a->must_equal(b)) {
        retval->taintedness(Taintedness::NO);
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
RiscOperators::ite(const BaseSemantics::SValue::Ptr &sel_,
                   const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr retval = SValue::promote(Super::ite(sel_, a_, b_));

    SValue::Ptr sel = SValue::promote(sel_);
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);

    if (sel->isBottom()) {
        retval->taintedness(mergeTaintedness(a, b));
    } else {
        const bool isA = retval->get_expression()->isEquivalentTo(a->get_expression());
        const bool isB = retval->get_expression()->isEquivalentTo(b->get_expression());
        if (isA && isB) {
            retval->taintedness(mergeTaintedness(a, b));
        } else if (isA) {
            retval->taintedness(a->taintedness());
        } else if (isB) {
            retval->taintedness(b->taintedness());
        } else {
            retval->taintedness(Taintedness::UNKNOWN);
        }
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
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValuePtr &sa) {
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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::SValue);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::TaintSemantics::RiscOperators);
#endif

#endif
