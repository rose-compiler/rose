//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmScalarType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmScalarType::~SgAsmScalarType() {
    destructorHelper();
}

SgAsmScalarType::SgAsmScalarType()
    : p_minorOrder(ByteOrder::ORDER_UNSPECIFIED)
    , p_majorOrder(ByteOrder::ORDER_UNSPECIFIED)
    , p_majorNBytes(0)
    , p_nBits(0) {}

void
SgAsmScalarType::initializeProperties() {
    p_minorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorNBytes = 0;
    p_nBits = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
