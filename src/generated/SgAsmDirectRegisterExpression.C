//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDirectRegisterExpression                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDirectRegisterExpression::~SgAsmDirectRegisterExpression() {
    destructorHelper();
}

SgAsmDirectRegisterExpression::SgAsmDirectRegisterExpression() {}

// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
SgAsmDirectRegisterExpression::SgAsmDirectRegisterExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor)
    : SgAsmRegisterReferenceExpression(descriptor) {}

void
SgAsmDirectRegisterExpression::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
