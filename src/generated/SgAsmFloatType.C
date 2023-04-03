#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmFloatType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmFloatType::~SgAsmFloatType() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmFloatType::SgAsmFloatType()
    : p_significandOffset((size_t)(-1))
    , p_significandNBits((size_t)(-1))
    , p_signBitOffset((size_t)(-1))
    , p_exponentOffset((size_t)(-1))
    , p_exponentNBits((size_t)(-1))
    , p_exponentBias(0)
    , p_flags(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
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

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
