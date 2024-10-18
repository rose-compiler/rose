#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <stringify.h>

using namespace Rose;

SgAsmScalarType::SgAsmScalarType(Rose::BinaryAnalysis::ByteOrder::Endianness sex, size_t nBits) {
    initializeProperties();
    p_minorOrder = sex;
    p_nBits = nBits;

    check();
}

// see super class
void
SgAsmScalarType::check() const {
    SgAsmType::check();
    ASSERT_always_require(p_nBits>0);
    ASSERT_always_require(p_nBits<=8 || p_minorOrder!=Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED);
    ASSERT_always_require(0==p_majorNBytes || p_majorOrder!=Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED);
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
        retval <<"majorOrder=" <<stringify::Rose::BinaryAnalysis::ByteOrder::Endianness(p_majorOrder, "")
               <<" over " <<p_majorNBytes <<"-byte units";
        retval <<"minorOrder=" <<stringify::Rose::BinaryAnalysis::ByteOrder::Endianness(p_minorOrder, "")
               <<" within " <<p_majorNBytes <<"-byte units";
    } else {
        retval <<"order=" <<stringify::Rose::BinaryAnalysis::ByteOrder::Endianness(p_minorOrder, "");
    }
    retval <<", nBits=" <<p_nBits <<")";
    return retval.str();
}

Rose::BinaryAnalysis::ByteOrder::Endianness
SgAsmScalarType::get_minorOrder() const {
    return p_minorOrder;
}

Rose::BinaryAnalysis::ByteOrder::Endianness
SgAsmScalarType::get_majorOrder() const {
    return p_majorOrder;
}

size_t
SgAsmScalarType::get_majorNBytes() const {
    return p_majorNBytes;
}

#endif
