#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmScalarType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmScalarType::~SgAsmScalarType() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmScalarType::SgAsmScalarType()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_minorOrder(ByteOrder::ORDER_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_majorOrder(ByteOrder::ORDER_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_majorNBytes(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_nBits(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmScalarType::initializeProperties() {
    p_minorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorNBytes = 0;
    p_nBits = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
