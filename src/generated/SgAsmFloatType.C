#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmFloatType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmFloatType::~SgAsmFloatType() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmFloatType::SgAsmFloatType()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_significandOffset((size_t)(-1))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_significandNBits((size_t)(-1))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_signBitOffset((size_t)(-1))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_exponentOffset((size_t)(-1))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_exponentNBits((size_t)(-1))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_exponentBias(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
