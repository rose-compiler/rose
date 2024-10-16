#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/StringUtility/Convert.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/SplitJoin.h>

#include <SgAsmFloatType.h>
#include <SgAsmInstruction.h>
#include <SgAsmIntegerType.h>

#include <Cxx_GrammarDowncast.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
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

RiscOperators::RiscOperators() {}

RiscOperators::RiscOperators(const SValue::Ptr &protoval, const SmtSolverPtr &solver)
    : protoval_(protoval), solver_(solver) {
    ASSERT_not_null(protoval_);
}

RiscOperators::RiscOperators(const State::Ptr &state, const SmtSolverPtr &solver)
    : currentState_(state), solver_(solver) {
    ASSERT_not_null(state);
    protoval_ = state->protoval();
}

RiscOperators::~RiscOperators() {}

void
RiscOperators::hash(Combinatorics::Hasher &hasher) {
    if (currentState())
        currentState()->hash(hasher, this, this);
}

SValue::Ptr
RiscOperators::undefined_(size_t nbits) {
    return protoval_->undefined_(nbits);
}

SValue::Ptr
RiscOperators::unspecified_(size_t nbits) {
    return protoval_->unspecified_(nbits);
}

SValue::Ptr
RiscOperators::number_(size_t nbits, uint64_t value) {
    return protoval_->number_(nbits, value);
}

SValue::Ptr
RiscOperators::boolean_(bool value) {
    return protoval_->boolean_(value);
}

SValue::Ptr
RiscOperators::bottom_(size_t nbits) {
    return protoval_->bottom_(nbits);
}

SValue::Ptr
RiscOperators::filterCallTarget(const SValue::Ptr &a) {
    return a->copy();
}

SValue::Ptr
RiscOperators::filterReturnTarget(const SValue::Ptr &a) {
    return a->copy();
}

SValue::Ptr
RiscOperators::filterIndirectJumpTarget(const SValue::Ptr &a) {
    return a->copy();
}


void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    SAWYER_MESG(mlog[TRACE]) <<"starting instruction " <<insn->toString() <<"\n";
    currentInsn_ = insn;
    ++nInsns_;
    isNoopRead_ = false;
};

void
RiscOperators::finishInstruction(SgAsmInstruction *insn) {
    ASSERT_always_not_null(insn);
    ASSERT_require(currentInsn_==insn);
    hotPatch_.apply(shared_from_this());
    currentInsn_ = nullptr;
    isNoopRead_ = false;
};

void
RiscOperators::comment(const std::string &comment) {
    if (mlog[DEBUG]) {
        std::vector<std::string> lines = StringUtility::split('\n', comment);
        while (!lines.empty() && lines.back().empty())
            lines.pop_back();
        if (SgAsmInstruction *insn = currentInstruction()) {
            if (lines.size() == 1) {
                lines[0] += " for instruction " + insn->toString();
            } else {
                lines.insert(lines.begin(), "for instruction " + insn->toString());
            }
        }
        for (const std::string &line: lines)
            mlog[DEBUG] <<"// " <<line <<"\n";
    }
}

std::pair<SValue::Ptr /*low*/, SValue::Ptr /*high*/>
RiscOperators::split(const SValue::Ptr &a, size_t splitPoint) {
    return std::make_pair(extract(a, 0, splitPoint), extract(a, splitPoint, a->nBits()));
}

SValue::Ptr
RiscOperators::addCarry(const SValue::Ptr &a, const SValue::Ptr &b,
                        SValue::Ptr &carryOut /*out*/, SValue::Ptr &overflowed /*out*/) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    ASSERT_require(a->nBits() == b->nBits());
    size_t nBits = a->nBits();
    SValue::Ptr carries;
    SValue::Ptr sum = addWithCarries(a, b, boolean_(false), carries /*out*/);
    carryOut = extract(carries, nBits-1, nBits);
    overflowed = xor_(carryOut, extract(carries, nBits-2, nBits-1));
    return sum;
}

SValue::Ptr
RiscOperators::subtract(const SValue::Ptr &minuend, const SValue::Ptr &subtrahend) {
    return add(minuend, negate(subtrahend));
}

SValue::Ptr
RiscOperators::subtractCarry(const SValue::Ptr &minuend, const SValue::Ptr &subtrahend,
                             SValue::Ptr &carryOut /*out*/, SValue::Ptr &overflowed /*out*/) {
    ASSERT_not_null(minuend);
    ASSERT_not_null(subtrahend);
    ASSERT_require(minuend->nBits() == subtrahend->nBits());
    size_t nBits = minuend->nBits();
    ASSERT_require(nBits > 1);
    SValue::Ptr negatedSubtrahend = negate(subtrahend);
    SValue::Ptr carries;
    SValue::Ptr difference = addWithCarries(minuend, negatedSubtrahend, boolean_(false), carries /*out*/);
    carryOut = extract(carries, nBits-1, nBits);
    overflowed = xor_(carryOut, extract(carries, nBits-2, nBits-1));
    return difference;
}

SValue::Ptr
RiscOperators::countLeadingZeros(const SValue::Ptr &a) {
    SValue::Ptr idx = mostSignificantSetBit(a);
    SValue::Ptr width = number_(a->nBits(), a->nBits());
    SValue::Ptr diff = subtract(number_(a->nBits(), a->nBits()-1), idx);
    return ite(equalToZero(a), width, diff);
}

SValue::Ptr
RiscOperators::countLeadingOnes(const SValue::Ptr &a) {
    return countLeadingZeros(invert(a));
}

SValue::Ptr
RiscOperators::reverseElmts(const SValue::Ptr &a, size_t elmtNBits) {
    ASSERT_not_null(a);
    ASSERT_require(elmtNBits > 0);
    ASSERT_require(elmtNBits <= a->nBits());
    ASSERT_require(a->nBits() % elmtNBits == 0);
    size_t nElmts = a->nBits() / elmtNBits;
    SValue::Ptr result;
    for (size_t i = 0; i < nElmts; ++i) {
        SValue::Ptr elmt = extract(a, i*elmtNBits, (i+1)*elmtNBits);
        result = result ? concatHiLo(result, elmt) : elmt;
    }
    ASSERT_require(result->nBits() == a->nBits());
    return result;
}

SValue::Ptr
RiscOperators::isEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return equalToZero(xor_(a, b));
}

SValue::Ptr
RiscOperators::isNotEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return invert(isEqual(a, b));
}

SValue::Ptr
RiscOperators::isUnsignedLessThan(const SValue::Ptr &a, const SValue::Ptr &b) {
    SValue::Ptr wideA = unsignedExtend(a, a->nBits()+1);
    SValue::Ptr wideB = unsignedExtend(b, b->nBits()+1);
    SValue::Ptr diff = subtract(wideA, wideB);
    return extract(diff, diff->nBits()-1, diff->nBits()); // A < B iff sign(wideA - wideB) == -1
}

SValue::Ptr
RiscOperators::isUnsignedLessThanOrEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return or_(isUnsignedLessThan(a, b), isEqual(a, b));
}

SValue::Ptr
RiscOperators::isUnsignedGreaterThan(const SValue::Ptr &a, const SValue::Ptr &b) {
    return invert(isUnsignedLessThanOrEqual(a, b));
}

SValue::Ptr
RiscOperators::isUnsignedGreaterThanOrEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return invert(isUnsignedLessThan(a, b));
}

SValue::Ptr
RiscOperators::isSignedLessThan(const SValue::Ptr &a, const SValue::Ptr &b) {
    ASSERT_require(a->nBits() == b->nBits());
    size_t nBits = a->nBits();
    SValue::Ptr wideA = signExtend(a, nBits+1);
    SValue::Ptr wideB = signExtend(b, nBits+1);
    SValue::Ptr difference = subtract(wideA, wideB);
    SValue::Ptr isNeg = extract(difference, nBits, nBits+1);
    return isNeg; // a < b implies a - b is negative
}

SValue::Ptr
RiscOperators::isSignedLessThanOrEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return or_(isSignedLessThan(a, b), isEqual(a, b));
}

SValue::Ptr
RiscOperators::isSignedGreaterThan(const SValue::Ptr &a, const SValue::Ptr &b) {
    return invert(isSignedLessThanOrEqual(a, b));
}

SValue::Ptr
RiscOperators::isSignedGreaterThanOrEqual(const SValue::Ptr &a, const SValue::Ptr &b) {
    return invert(isSignedLessThan(a, b));
}

SValue::Ptr
RiscOperators::unsignedExtend(const SValue::Ptr &a, size_t new_width) {
    return a->copy(new_width);
}

void
RiscOperators::interrupt(const int major, const int minor) {
    if (currentState()->hasInterruptState()) {
        ASSERT_require(major >= 0);
        ASSERT_require(minor >= 0);
        currentState()->raiseInterrupt(major, minor, this);
    } else {
        // old behavior was to do nothing
    }
}

void
RiscOperators::raiseInterrupt(const unsigned majorNumber, const unsigned minorNumber, const SValue::Ptr &raise) {
    ASSERT_not_null(raise);
    ASSERT_require(raise->nBits() == 1);
    ASSERT_not_null(currentState());

    if (SValue::Ptr oldValue = currentState()->readInterrupt(majorNumber, minorNumber, boolean_(false), this)) {
        SValue::Ptr newValue = or_(oldValue, raise);
        currentState()->writeInterrupt(majorNumber, minorNumber, newValue, this);
    } else if (raise->isTrue()) {
        interrupt(majorNumber, minorNumber);
    } else if (raise->isFalse()) {
        // don't raise an interrupt
    } else {
        throw Exception("interrupt enabled value must be concrete for default implementation", currentInstruction());
    }
}

void
RiscOperators::interrupt(const SValue::Ptr &majorNumber, const SValue::Ptr &minorNumber, const SValue::Ptr &raise) {
    ASSERT_not_null(majorNumber);
    ASSERT_not_null(minorNumber);
    ASSERT_not_null(raise);
    ASSERT_require(raise->nBits() == 1);

    if (!majorNumber->isConcrete())
        throw Exception("interrupt major number must be concrete for default implementation", currentInstruction());
    if (!minorNumber->isConcrete())
        throw Exception("interrupt minor number must be concrete for default implementation", currentInstruction());
    raiseInterrupt(majorNumber->toUnsigned().get(), minorNumber->toUnsigned().get(), raise);
}

SValue::Ptr
RiscOperators::fpFromInteger(const SValue::Ptr &/*intValue*/, SgAsmFloatType *fpType) {
    ASSERT_always_not_null(fpType);
    throw NotImplemented("fpFromInteger is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpToInteger(const SValue::Ptr &/*fpValue*/, SgAsmFloatType *fpType, const SValue::Ptr &/*dflt*/) {
    ASSERT_always_not_null(fpType);
    throw NotImplemented("fpToInteger is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpConvert(const SValue::Ptr &a, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_not_null(retType);
    if (aType == retType)
        return a->copy();
    throw NotImplemented("fpConvert is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpIsNan(const SValue::Ptr &a, SgAsmFloatType *aType) {
    // Value is NAN iff exponent bits are all set and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValue::Ptr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValue::Ptr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), invert(equalToZero(significand)));
}

SValue::Ptr
RiscOperators::fpIsDenormalized(const SValue::Ptr &a, SgAsmFloatType *aType) {
    // Value is denormalized iff exponent is zero and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    if (!aType->gradualUnderflow())
        return boolean_(false);
    SValue::Ptr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValue::Ptr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), invert(equalToZero(significand)));
}

SValue::Ptr
RiscOperators::fpIsZero(const SValue::Ptr &a, SgAsmFloatType *aType) {
    // Value is zero iff exponent and significand are both zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValue::Ptr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValue::Ptr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), equalToZero(significand));
}

SValue::Ptr
RiscOperators::fpIsInfinity(const SValue::Ptr &a, SgAsmFloatType *aType) {
    // Value is infinity iff exponent bits are all set and significand is zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValue::Ptr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValue::Ptr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), equalToZero(significand));
}

SValue::Ptr
RiscOperators::fpSign(const SValue::Ptr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    return extract(a, aType->signBit(), aType->signBit()+1);
}

SValue::Ptr
RiscOperators::fpEffectiveExponent(const SValue::Ptr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    size_t expWidth = aType->exponentBits().size() + 1; // add a sign bit to the beginning
    SValue::Ptr storedExponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValue::Ptr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    SValue::Ptr retval = ite(equalToZero(storedExponent),
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

SValue::Ptr
RiscOperators::fpAdd(const SValue::Ptr&, const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpAdd is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpSubtract(const SValue::Ptr&, const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpSubtract is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpMultiply(const SValue::Ptr&, const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpMultiply is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpDivide(const SValue::Ptr&, const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpDivide is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpSquareRoot(const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpSquareRoot is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::fpRoundTowardZero(const SValue::Ptr&, SgAsmFloatType*) {
    throw NotImplemented("fpRoundTowardZero is not implemented", currentInstruction());
}

SValue::Ptr
RiscOperators::convert(const SValue::Ptr &a, SgAsmType *srcType, SgAsmType *dstType) {
    ASSERT_not_null(a);
    ASSERT_not_null(srcType);
    ASSERT_not_null(dstType);
    SValue::Ptr srcVal = reinterpret(a, srcType);
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

SValue::Ptr
RiscOperators::reinterpret(const SValue::Ptr &a, SgAsmType *type) {
    ASSERT_not_null(a);
    ASSERT_not_null(type);
    if (a->nBits() != type->get_nBits()) {
        throw Exception("reinterpret type has different size (" + StringUtility::plural(type->get_nBits(), "bits") + ")"
                        " than value (" + StringUtility::plural(a->nBits(), "bits") + ")", currentInsn_);
    }
    return a->copy();
}

SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const SValue::Ptr &dflt_) {
    SValue::Ptr dflt = dflt_;
    ASSERT_not_null(currentState());
    ASSERT_not_null(dflt);

    // If there's an lazily-updated initial state, then get its register, updating the initial state as a side effect.
    if (initialState_)
        dflt = initialState()->readRegister(reg, dflt, this);

    SValue::Ptr retval = currentState()->readRegister(reg, dflt, this);
    currentState()->registerState()->updateReadProperties(reg);
    return retval;
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const SValue::Ptr &a) {
    ASSERT_not_null(currentState());
    currentState()->writeRegister(reg, a, this);
    currentState()->registerState()->updateWriteProperties(reg, currentInstruction() ? IO_WRITE : IO_INIT);
}

SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const SValue::Ptr &dflt_) {
    SValue::Ptr dflt = dflt_;
    ASSERT_not_null(currentState());
    ASSERT_not_null(dflt);
    return currentState()->peekRegister(reg, dflt, this);
}

void
RiscOperators::print(std::ostream &stream, const std::string prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

void
RiscOperators::print(std::ostream &stream, Formatter &fmt) const {
    currentState()->print(stream, fmt);
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

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RiscOperators);

#endif
