//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryMultiply            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryMultiply_IMPL
#include <SgAsmBinaryMultiply.h>

SgAsmBinaryMultiply::~SgAsmBinaryMultiply() {
    destructorHelper();
}

SgAsmBinaryMultiply::SgAsmBinaryMultiply() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryMultiply::SgAsmBinaryMultiply(SgAsmExpression* const& lhs,
                                         SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryMultiply::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
