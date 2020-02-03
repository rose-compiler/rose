#include <sage3basic.h>
#include <BaseSemanticsRiscOperators.h>

#include <BaseSemanticsException.h>
#include <BaseSemanticsFormatter.h>
#include <BaseSemanticsState.h>
#include <BaseSemanticsSValue.h>
#include <InstructionSemantics2.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

std::ostream&
operator<<(std::ostream &o, const RiscOperators &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const RiscOperators::WithFormatter &x) {
    x.print(o);
    return o;
}

RiscOperators::RiscOperators()
    : currentInsn_(NULL), nInsns_(0) {}

RiscOperators::RiscOperators(const SValuePtr &protoval, const SmtSolverPtr &solver)
    : protoval_(protoval), solver_(solver), currentInsn_(NULL), nInsns_(0) {
    ASSERT_not_null(protoval_);
}

RiscOperators::RiscOperators(const StatePtr &state, const SmtSolverPtr &solver)
    : currentState_(state), solver_(solver), currentInsn_(NULL), nInsns_(0) {
    ASSERT_not_null(state);
    protoval_ = state->protoval();
}

RiscOperators::~RiscOperators() {}

SValuePtr
RiscOperators::undefined_(size_t nbits) {
    return protoval_->undefined_(nbits);
}

SValuePtr
RiscOperators::unspecified_(size_t nbits) {
    return protoval_->unspecified_(nbits);
}

SValuePtr
RiscOperators::number_(size_t nbits, uint64_t value) {
    return protoval_->number_(nbits, value);
}

SValuePtr
RiscOperators::boolean_(bool value) {
    return protoval_->boolean_(value);
}

SValuePtr
RiscOperators::bottom_(size_t nbits) {
    return protoval_->bottom_(nbits);
}

SValuePtr
RiscOperators::filterCallTarget(const SValuePtr &a) {
    return a->copy();
}

SValuePtr
RiscOperators::filterReturnTarget(const SValuePtr &a) {
    return a->copy();
}

SValuePtr
RiscOperators::filterIndirectJumpTarget(const SValuePtr &a) {
    return a->copy();
}


void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    SAWYER_MESG(mlog[TRACE]) <<"starting instruction " <<insn->toString() <<"\n";
    currentInsn_ = insn;
    ++nInsns_;
};

void
RiscOperators::finishInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    ASSERT_require(currentInsn_==insn);
    hotPatch_.apply(shared_from_this());
    currentInsn_ = NULL;
};

std::pair<SValuePtr /*low*/, SValuePtr /*high*/>
RiscOperators::split(const SValuePtr &a, size_t splitPoint) {
    return std::make_pair(extract(a, 0, splitPoint), extract(a, splitPoint, a->get_width()));
}

SValuePtr
RiscOperators::addCarry(const SValuePtr &a, const SValuePtr &b,
                        SValuePtr &carryOut /*out*/, SValuePtr &overflowed /*out*/) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    ASSERT_require(a->get_width() == b->get_width());
    size_t nBits = a->get_width();
    SValuePtr carries;
    SValuePtr sum = addWithCarries(a, b, boolean_(false), carries /*out*/);
    carryOut = extract(carries, nBits-1, nBits);
    overflowed = xor_(carryOut, extract(carries, nBits-2, nBits-1));
    return sum;
}

SValuePtr
RiscOperators::subtract(const SValuePtr &minuend, const SValuePtr &subtrahend) {
    return add(minuend, negate(subtrahend));
}

SValuePtr
RiscOperators::subtractCarry(const SValuePtr &minuend, const SValuePtr &subtrahend,
                             SValuePtr &carryOut /*out*/, SValuePtr &overflowed /*out*/) {
    ASSERT_not_null(minuend);
    ASSERT_not_null(subtrahend);
    ASSERT_require(minuend->get_width() == subtrahend->get_width());
    size_t nBits = minuend->get_width();
    ASSERT_require(nBits > 1);
    SValuePtr negatedSubtrahend = negate(subtrahend);
    SValuePtr carries;
    SValuePtr difference = addWithCarries(minuend, negatedSubtrahend, boolean_(false), carries /*out*/);
    carryOut = extract(carries, nBits-1, nBits);
    overflowed = xor_(carryOut, extract(carries, nBits-2, nBits-1));
    return difference;
}

SValuePtr
RiscOperators::isEqual(const SValuePtr &a, const SValuePtr &b) {
    return equalToZero(xor_(a, b));
}

SValuePtr
RiscOperators::isNotEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedLessThan(const SValuePtr &a, const SValuePtr &b) {
    SValuePtr wideA = unsignedExtend(a, a->get_width()+1);
    SValuePtr wideB = unsignedExtend(b, b->get_width()+1);
    SValuePtr diff = subtract(wideA, wideB);
    return extract(diff, diff->get_width()-1, diff->get_width()); // A < B iff sign(wideA - wideB) == -1
}

SValuePtr
RiscOperators::isUnsignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return or_(isUnsignedLessThan(a, b), isEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedGreaterThan(const SValuePtr &a, const SValuePtr &b) {
    return invert(isUnsignedLessThanOrEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isUnsignedLessThan(a, b));
}

SValuePtr
RiscOperators::isSignedLessThan(const SValuePtr &a, const SValuePtr &b) {
    ASSERT_require(a->get_width() == b->get_width());
    size_t nBits = a->get_width();
    SValuePtr wideA = signExtend(a, nBits+1);
    SValuePtr wideB = signExtend(b, nBits+1);
    SValuePtr difference = subtract(wideA, wideB);
    SValuePtr isNeg = extract(difference, nBits, nBits+1);
    return isNeg; // a < b implies a - b is negative
}

SValuePtr
RiscOperators::isSignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return or_(isSignedLessThan(a, b), isEqual(a, b));
}

SValuePtr
RiscOperators::isSignedGreaterThan(const SValuePtr &a, const SValuePtr &b) {
    return invert(isSignedLessThanOrEqual(a, b));
}

SValuePtr
RiscOperators::isSignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isSignedLessThan(a, b));
}

SValuePtr
RiscOperators::unsignedExtend(const SValuePtr &a, size_t new_width) {
    return a->copy(new_width);
}

SValuePtr
RiscOperators::fpFromInteger(const SValuePtr &intValue, SgAsmFloatType *fpType) {
    ASSERT_not_null(fpType);
    throw NotImplemented("fpFromInteger is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpToInteger(const SValuePtr &fpValue, SgAsmFloatType *fpType, const SValuePtr &dflt) {
    ASSERT_not_null(fpType);
    throw NotImplemented("fpToInteger is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpConvert(const SValuePtr &a, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_not_null(retType);
    if (aType == retType)
        return a->copy();
    throw NotImplemented("fpConvert is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpIsNan(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is NAN iff exponent bits are all set and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), invert(equalToZero(significand)));
}

SValuePtr
RiscOperators::fpIsDenormalized(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is denormalized iff exponent is zero and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    if (!aType->gradualUnderflow())
        return boolean_(false);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), invert(equalToZero(significand)));
}

SValuePtr
RiscOperators::fpIsZero(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is zero iff exponent and significand are both zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), equalToZero(significand));
}

SValuePtr
RiscOperators::fpIsInfinity(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is infinity iff exponent bits are all set and significand is zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), equalToZero(significand));
}

SValuePtr
RiscOperators::fpSign(const SValuePtr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    return extract(a, aType->signBit(), aType->signBit()+1);
}

SValuePtr
RiscOperators::fpEffectiveExponent(const SValuePtr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    size_t expWidth = aType->exponentBits().size() + 1; // add a sign bit to the beginning
    SValuePtr storedExponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    SValuePtr retval = ite(equalToZero(storedExponent),
                           ite(equalToZero(significand),
                               // Stored exponent and significand are both zero, therefore value is zero
                               number_(expWidth, 0),    // value is zero, therefore exponent is zero

                               // Stored exponent is zero but significand is not, therefore denormalized number.
                               // effective exponent is 1 - bias - (significandWidth - mssb(significand))
                               add(number_(expWidth, 1 - aType->exponentBias() - aType->significandBits().size()),
                                   unsignedExtend(mostSignificantSetBit(significand), expWidth))),

                           // Stored exponent is non-zero so significand is normalized. Effective exponent is the stored
                           // exponent minus the bias.
                           subtract(unsignedExtend(storedExponent, expWidth),
                                    number_(expWidth, aType->exponentBias())));
    return retval;
}

SValuePtr
RiscOperators::fpAdd(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpAdd is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpSubtract(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpSubtract is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpMultiply(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpMultiply is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpDivide(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpDivide is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpSquareRoot(const SValuePtr &a, SgAsmFloatType *aType) {
    throw NotImplemented("fpSquareRoot is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::fpRoundTowardZero(const SValuePtr &a, SgAsmFloatType *aType) {
    throw NotImplemented("fpRoundTowardZero is not implemented", currentInstruction());
}

SValuePtr
RiscOperators::convert(const SValuePtr &a, SgAsmType *srcType, SgAsmType *dstType) {
    ASSERT_not_null(a);
    ASSERT_not_null(srcType);
    ASSERT_not_null(dstType);
    SValuePtr srcVal = reinterpret(a, srcType);
    if (srcType == dstType)
        return srcVal;

    //--------------------------------
    // Integer to integer conversions
    //--------------------------------

    SgAsmIntegerType *iSrcType = isSgAsmIntegerType(srcType);
    SgAsmIntegerType *iDstType = isSgAsmIntegerType(dstType);
    if (iSrcType && iDstType) {
        if (!iSrcType->get_isSigned() && !iDstType->get_isSigned()) {
            // unsigned -> unsigned (overflows truncated for decreasing width)
            return reinterpret(unsignedExtend(srcVal, dstType->get_nBits()), dstType);
        } else if (!iSrcType->get_isSigned()) {
            ASSERT_require(iDstType->get_isSigned());
            if (dstType->get_nBits() > srcType->get_nBits()) {
                // unsigned -> signed, increasing width (no overflow possible)
                return reinterpret(unsignedExtend(srcVal, dstType->get_nBits()), dstType);
            } else {
                ASSERT_require(dstType->get_nBits() < srcType->get_nBits());
                // unsigned -> signed, decreasing width (overflows truncated)
                return reinterpret(unsignedExtend(srcVal, dstType->get_nBits()), dstType);
            }
        } else if (!iDstType->get_isSigned()) {
            ASSERT_require(iSrcType->get_isSigned());
            // signed -> unsigned (overflows truncated)
            return reinterpret(unsignedExtend(srcVal, dstType->get_nBits()), dstType);
        } else {
            ASSERT_require(iSrcType->get_isSigned());
            ASSERT_require(iDstType->get_isSigned());
            if (dstType->get_nBits() >= srcType->get_nBits()) {
                // signed -> signed, increasing width (no overflow possible)
                return reinterpret(signExtend(srcVal, dstType->get_nBits()), dstType);
            } else {
                // signed -> signed, decreasing width (overflows truncated)
                return reinterpret(unsignedExtend(srcVal, dstType->get_nBits()), dstType);
            }
        }
    }

    //--------------------------------
    // FP to FP conversions
    //--------------------------------

    SgAsmFloatType *fpSrcType = isSgAsmFloatType(srcType);
    SgAsmFloatType *fpDstType = isSgAsmFloatType(dstType);
    if (fpSrcType && fpDstType)
        return fpConvert(srcVal, fpSrcType, fpDstType);

    //--------------------------------
    // Integer to FP conversions
    //--------------------------------

    if (iSrcType && fpDstType)
        throw Exception("unable to convert from integer to floating-point", currentInsn_);

    //--------------------------------
    // FP to integer conversions
    //--------------------------------

    if (fpSrcType && iDstType)
        throw Exception("unable to convert from floating-point to integer", currentInsn_);

    //--------------------------------
    // Vector conversions
    //--------------------------------

    SgAsmVectorType *vSrcType = isSgAsmVectorType(srcType);
    SgAsmVectorType *vDstType = isSgAsmVectorType(dstType);
    if (vSrcType)
        throw Exception("unable to convert from vector type", currentInsn_);
    if (vDstType)
        throw Exception("unable to convert to vector type", currentInsn_);

    //--------------------------------
    // Catch-all
    //--------------------------------

    throw Exception("unable to convert between specified types", currentInsn_);
}

SValuePtr
RiscOperators::reinterpret(const SValuePtr &a, SgAsmType *type) {
    ASSERT_not_null(a);
    ASSERT_not_null(type);
    if (a->get_width() != type->get_nBits()) {
        throw Exception("reinterpret type has different size (" + StringUtility::plural(type->get_nBits(), "bits") + ")"
                        " than value (" + StringUtility::plural(a->get_width(), "bits") + ")", currentInsn_);
    }
    return a->copy();
}

SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const SValuePtr &dflt_) {
    SValuePtr dflt = dflt_;
    ASSERT_not_null(currentState_);
    ASSERT_not_null(dflt);

    // If there's an lazily-updated initial state, then get its register, updating the initial state as a side effect.
    if (initialState_)
        dflt = initialState()->readRegister(reg, dflt, this);

    return currentState_->readRegister(reg, dflt, this);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const SValuePtr &a) {
    ASSERT_not_null(currentState_);
    currentState_->writeRegister(reg, a, this);
}

SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const SValuePtr &dflt_) {
    SValuePtr dflt = dflt_;
    ASSERT_not_null(currentState_);
    ASSERT_not_null(dflt);
    return currentState_->peekRegister(reg, dflt, this);
}

void
RiscOperators::print(std::ostream &stream, const std::string prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

void
RiscOperators::print(std::ostream &stream, Formatter &fmt) const {
    currentState_->print(stream, fmt);
}

RiscOperators::WithFormatter
RiscOperators::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

} // namespace
} // namespace
} // namespace
} // namespace
