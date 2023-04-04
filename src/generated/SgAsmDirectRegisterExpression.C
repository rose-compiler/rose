#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDirectRegisterExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDirectRegisterExpression::~SgAsmDirectRegisterExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDirectRegisterExpression::SgAsmDirectRegisterExpression() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmDirectRegisterExpression::SgAsmDirectRegisterExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor)
    : SgAsmRegisterReferenceExpression(descriptor) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDirectRegisterExpression::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
