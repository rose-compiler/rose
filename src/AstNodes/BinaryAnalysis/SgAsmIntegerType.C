#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;

/** Construct a new integer type. */
SgAsmIntegerType::SgAsmIntegerType(Rose::BinaryAnalysis::ByteOrder::Endianness sex, size_t nBits, bool isSigned)
    : SgAsmScalarType(sex, nBits) {
    initializeProperties();
    p_isSigned = isSigned;
    check();
    if (1==nBits)
        isSigned = false;
}

SgAsmIntegerType*
SgAsmIntegerType::instanceUnsigned(Rose::BinaryAnalysis::ByteOrder::Endianness order, size_t nBits) {
    return new SgAsmIntegerType(order, nBits, false);
}

SgAsmIntegerType*
SgAsmIntegerType::instanceSigned(Rose::BinaryAnalysis::ByteOrder::Endianness order, size_t nBits) {
    return new SgAsmIntegerType(order, nBits, true);
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

#endif
