#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmScalarType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmScalarType::~SgAsmScalarType() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmScalarType::SgAsmScalarType()
    : p_minorOrder(ByteOrder::ORDER_UNSPECIFIED)
    , p_majorOrder(ByteOrder::ORDER_UNSPECIFIED)
    , p_majorNBytes(0)
    , p_nBits(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmScalarType::initializeProperties() {
    p_minorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorOrder = ByteOrder::ORDER_UNSPECIFIED;
    p_majorNBytes = 0;
    p_nBits = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
