#include "sage3basic.h"
#include "stringify.h"

using namespace Rose;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// All known types
// FIXME[Robb P. Matzke 2014-07-21]: deleting a type should remove the type from the registry
Sawyer::Container::Map<std::string, SgAsmType*> SgAsmType::p_typeRegistry;

/** Check internal consistency of a type. */
void
SgAsmType::check() const {}

/** Returns the number of bytes require to represent an instance of this type.  Types are allowed to have a size in bits which
 *  is not a multiple of eight; this method will round such sizes up to the next byte boundary. */
size_t
SgAsmType::get_nBytes() const {
    return (get_nBits() + 7) / 8;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmScalarType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmScalarType::SgAsmScalarType(ByteOrder::Endianness sex, size_t nBits)
    : p_minorOrder(sex), p_majorOrder(ByteOrder::ORDER_UNSPECIFIED), p_majorNBytes(0), p_nBits(nBits) {
    check();
    if (p_nBits<=8)
        p_minorOrder = ByteOrder::ORDER_UNSPECIFIED;
    if (p_majorNBytes==0 || p_majorNBytes<=get_nBytes())
        p_majorOrder = ByteOrder::ORDER_UNSPECIFIED;
}

// see super class
void
SgAsmScalarType::check() const {
    SgAsmType::check();
    ASSERT_always_require(p_nBits>0);
    ASSERT_always_require(p_nBits<=8 || p_minorOrder!=ByteOrder::ORDER_UNSPECIFIED);
    ASSERT_always_require(0==p_majorNBytes || p_majorOrder!=ByteOrder::ORDER_UNSPECIFIED);
    ASSERT_always_require(0==p_majorNBytes || p_majorNBytes>1);
}

// see super class
size_t
SgAsmScalarType::get_nBits() const {
    return p_nBits;
}

// see super class
std::string
SgAsmScalarType::toString() const {
    std::ostringstream retval;
    retval <<"scalar(";
    if (p_majorNBytes > 0) {
        retval <<"majorOrder=" <<stringifyByteOrderEndianness(p_majorOrder) <<" over " <<p_majorNBytes <<"-byte units";
        retval <<"minorOrder=" <<stringifyByteOrderEndianness(p_minorOrder) <<" within " <<p_majorNBytes <<"-byte units";
    } else {
        retval <<"order=" <<stringifyByteOrderEndianness(p_minorOrder);
    }
    retval <<", nBits=" <<p_nBits <<")";
    return retval.str();
}

ByteOrder::Endianness
SgAsmScalarType::get_minorOrder() const {
    return p_minorOrder;
}

ByteOrder::Endianness
SgAsmScalarType::get_majorOrder() const {
    return p_majorOrder;
}

size_t
SgAsmScalarType::get_majorNBytes() const {
    return p_majorNBytes;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmIntegerType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Construct a new integer type. */
SgAsmIntegerType::SgAsmIntegerType(ByteOrder::Endianness sex, size_t nBits, bool isSigned)
    : SgAsmScalarType(sex, nBits), p_isSigned(isSigned) {
    check();
    if (1==nBits)
        isSigned = false;
}

// see super class
void
SgAsmIntegerType::check() const {
    SgAsmScalarType::check();
}

// see super class
std::string
SgAsmIntegerType::toString() const {
    std::ostringstream retval;
    retval <<"integer(" <<(p_isSigned?"signed":"unsigned") <<", " <<SgAsmScalarType::toString() <<")";
    return retval.str();
}

bool
SgAsmIntegerType::get_isSigned() const {
    return p_isSigned;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmFloatType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmFloatType::SgAsmFloatType(ByteOrder::Endianness sex, size_t nBits,
                               const BitRange &significandBits, const BitRange exponentBits, size_t signBit,
                               uint64_t exponentBias, Flags flags)
    : SgAsmScalarType(sex, nBits), p_signBitOffset(signBit), p_exponentBias(exponentBias), p_flags(flags.vector()) {
    ASSERT_forbid(significandBits.isEmpty());
    ASSERT_forbid(exponentBits.isEmpty());
    p_significandOffset = significandBits.least();
    p_significandNBits = significandBits.size();
    p_exponentOffset = exponentBits.least();
    p_exponentNBits = exponentBits.size();
    check();
}

// see super class
void
SgAsmFloatType::check() const {
    SgAsmScalarType::check();
    ASSERT_always_forbid(exponentBits().isOverlapping(significandBits()));
    ASSERT_always_forbid(exponentBits().isOverlapping(signBit()));
    ASSERT_always_forbid(significandBits().isOverlapping(signBit()));
    ASSERT_always_require(exponentBits().greatest() < get_nBits());
    ASSERT_always_require(significandBits().greatest() < get_nBits());
    ASSERT_always_require(signBit() < get_nBits());
}

// see super class
std::string
SgAsmFloatType::toString() const {
    std::ostringstream retval;
    retval <<"float=("
           <<"significand=[" <<p_significandOffset <<"+" <<p_significandNBits <<"], "
           <<"sign=[" <<p_signBitOffset <<"+1], "
           <<"exponent=[" <<p_exponentOffset <<"+" <<p_exponentNBits <<"], "
           <<"bias=" <<StringUtility::toHex(p_exponentBias) <<", ";
    if (gradualUnderflow())
        retval <<"gradual-underflow, ";
    if (implicitBitConvention())
        retval <<"implied-bit-convention, ";
    retval <<SgAsmScalarType::toString() <<")";
    return retval.str();
};

SgAsmFloatType::BitRange
SgAsmFloatType::significandBits() const {
    return BitRange::baseSize(p_significandOffset, p_significandNBits);
}

SgAsmFloatType::BitRange
SgAsmFloatType::exponentBits() const {
    return BitRange::baseSize(p_exponentOffset, p_exponentNBits);
}

size_t
SgAsmFloatType::signBit() const {
    return p_signBitOffset;
}

uint64_t
SgAsmFloatType::exponentBias() const {
    return p_exponentBias;
}

// class method
SgAsmFloatType::Flags
SgAsmFloatType::ieeeFlags() {
    return GRADUAL_UNDERFLOW | IMPLICIT_BIT_CONVENTION;
}

SgAsmFloatType::Flags
SgAsmFloatType::flags() const {
    return Flags(p_flags);
}

bool
SgAsmFloatType::gradualUnderflow() const {
    return flags().isSet(GRADUAL_UNDERFLOW);
}

bool
SgAsmFloatType::implicitBitConvention() const {
    return flags().isSet(IMPLICIT_BIT_CONVENTION);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmVectorType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmVectorType::SgAsmVectorType(size_t nElmts, SgAsmType *elmtType): p_nElmts(nElmts), p_elmtType(elmtType) {
    check();
}

// see super class
void
SgAsmVectorType::check() const {
    SgAsmType::check();
    ASSERT_always_require(p_nElmts>0);
    ASSERT_always_not_null(p_elmtType);
    p_elmtType->check();
}

// see super class
std::string
SgAsmVectorType::toString() const {
    std::ostringstream retval;
    retval <<"vector(nElmts=" <<p_nElmts <<", " <<p_elmtType->toString() <<")";
    return retval.str();
}

// see super class
size_t
SgAsmVectorType::get_nBits() const {
    // each element is aligned on a byte boundary
    ASSERT_require(p_nElmts>0);
    return 8 * (p_nElmts-1) * p_elmtType->get_nBytes() + p_elmtType->get_nBits();
}

size_t
SgAsmVectorType::get_nElmts() const {
    return p_nElmts;
}

SgAsmType *
SgAsmVectorType::get_elmtType() const {
    return p_elmtType;
}
