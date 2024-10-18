#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;

SgAsmFloatType::SgAsmFloatType(Rose::BinaryAnalysis::ByteOrder::Endianness sex, size_t nBits,
                               const BitRange &significandBits, const BitRange exponentBits, size_t signBit,
                               uint64_t exponentBias, Flags flags)
    : SgAsmScalarType(sex, nBits) {
    initializeProperties();

    p_signBitOffset = signBit;
    p_exponentBias = exponentBias;
    p_flags = flags.vector();

    ASSERT_forbid(significandBits.isEmpty());
    ASSERT_forbid(exponentBits.isEmpty());
    p_significandOffset = significandBits.least();
    p_significandNBits = significandBits.size();
    p_exponentOffset = exponentBits.least();
    p_exponentNBits = exponentBits.size();
    check();
}

SgAsmFloatType*
SgAsmFloatType::instanceIeee32(Rose::BinaryAnalysis::ByteOrder::Endianness sex) {
    return new SgAsmFloatType(sex, 32,
                              SgAsmFloatType::BitRange::baseSize(0, 23), // significand
                              SgAsmFloatType::BitRange::baseSize(23, 8), // exponent
                              31,                                        // sign bit
                              127,                                       // exponent bias
                              SgAsmFloatType::ieeeFlags());
}

SgAsmFloatType*
SgAsmFloatType::instanceIeee64(Rose::BinaryAnalysis::ByteOrder::Endianness sex) {
    return new SgAsmFloatType(sex, 64,
                              SgAsmFloatType::BitRange::baseSize(0, 52),  // significand
                              SgAsmFloatType::BitRange::baseSize(52, 11), // exponent
                              63,                                         // sign bit
                              1023,                                       // exponent bias
                              SgAsmFloatType::ieeeFlags());
}

// see super class
void
SgAsmFloatType::check() const {
    SgAsmScalarType::check();
    ASSERT_always_forbid(exponentBits().overlaps(significandBits()));
    ASSERT_always_forbid(exponentBits().overlaps(signBit()));
    ASSERT_always_forbid(significandBits().overlaps(signBit()));
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

#endif
