//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnarySignedExtend            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnarySignedExtend_IMPL
#include <SgAsmUnarySignedExtend.h>

SgAsmUnarySignedExtend::~SgAsmUnarySignedExtend() {
    destructorHelper();
}

SgAsmUnarySignedExtend::SgAsmUnarySignedExtend() {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnarySignedExtend::SgAsmUnarySignedExtend(SgAsmExpression* const& operand)
    : SgAsmUnaryExpression(operand) {}

void
SgAsmUnarySignedExtend::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
