//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmFloatType                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmFloatType::~SgAsmFloatType() {
    destructorHelper();
}

SgAsmFloatType::SgAsmFloatType()
    : p_significandOffset((size_t)(-1))
    , p_significandNBits((size_t)(-1))
    , p_signBitOffset((size_t)(-1))
    , p_exponentOffset((size_t)(-1))
    , p_exponentNBits((size_t)(-1))
    , p_exponentBias(0)
    , p_flags(0) {}

void
SgAsmFloatType::initializeProperties() {
    p_significandOffset = (size_t)(-1);
    p_significandNBits = (size_t)(-1);
    p_signBitOffset = (size_t)(-1);
    p_exponentOffset = (size_t)(-1);
    p_exponentNBits = (size_t)(-1);
    p_exponentBias = 0;
    p_flags = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
