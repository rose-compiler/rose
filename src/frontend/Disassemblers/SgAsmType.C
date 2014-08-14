#include "sage3basic.h"
#include "stringify.h"

using namespace rose;

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

/** Construct a new scalar type.  Since scalar types are base classes, one normally does not construct just a scalar type but
 *  rather one of the base classes. */
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

/** Construct a new floating-point type. */
SgAsmFloatType::SgAsmFloatType(ByteOrder::Endianness sex, size_t nBits,
                               size_t significandOffset, size_t significandNBits, size_t signBitOffset,
                               size_t exponentOffset, size_t exponentNBits, uint64_t exponentBias)
    : SgAsmScalarType(sex, nBits), p_significandOffset(significandOffset), p_significandNBits(significandNBits),
      p_signBitOffset(signBitOffset), p_exponentOffset(exponentOffset), p_exponentNBits(exponentNBits),
      p_exponentBias(exponentBias) {
    check();
}

// see super class
void
SgAsmFloatType::check() const {
    SgAsmScalarType::check();
    ASSERT_always_require(p_significandOffset < p_nBits);
    ASSERT_always_require(p_significandNBits > 0);
    ASSERT_always_require(p_significandOffset + p_significandNBits <= p_nBits);
    ASSERT_always_require(p_signBitOffset < p_nBits);
    ASSERT_always_require(p_exponentOffset < p_nBits);
    ASSERT_always_require(p_exponentOffset + p_exponentNBits <= p_nBits);

    typedef Sawyer::Container::Interval<size_t> BitRange;
    BitRange exponentRegion = BitRange::baseSize(p_exponentOffset, p_exponentNBits);
    BitRange significandRegion = BitRange::baseSize(p_significandOffset, p_significandNBits);
    BitRange signBit = BitRange::baseSize(p_signBitOffset, 1);
    ASSERT_always_forbid(exponentRegion.isOverlapping(significandRegion));
    ASSERT_always_forbid(exponentRegion.isOverlapping(signBit));
    ASSERT_always_forbid(significandRegion.isOverlapping(signBit));
}

// see super class
std::string
SgAsmFloatType::toString() const {
    std::ostringstream retval;
    retval <<"float=("
           <<"significand=[" <<p_significandOffset <<"+" <<p_significandNBits <<"], "
           <<"sign=[" <<p_signBitOffset <<"+1], "
           <<"exponent=[" <<p_exponentOffset <<"+" <<p_exponentNBits <<"], "
           <<"bias=" <<StringUtility::toHex(p_exponentBias) <<", "
           <<SgAsmScalarType::toString() <<")";
    return retval.str();
};

size_t
SgAsmFloatType::get_significandOffset() const {
    return p_significandOffset;
}

size_t
SgAsmFloatType::get_significandNBits() const {
    return p_significandNBits;
}

size_t
SgAsmFloatType::get_signBitOffset() const {
    return p_signBitOffset;
}

size_t
SgAsmFloatType::get_exponentOffset() const {
    return p_exponentOffset;
}

size_t
SgAsmFloatType::get_exponentNBits() const {
    return p_exponentNBits;
}

uint64_t
SgAsmFloatType::get_exponentBias() const {
    return p_exponentBias;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmVectorType
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Construct a new vector type. */
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
