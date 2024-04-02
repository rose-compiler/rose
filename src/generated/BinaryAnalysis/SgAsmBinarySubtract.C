//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinarySubtract            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinarySubtract_IMPL
#include <SgAsmBinarySubtract.h>

SgAsmBinarySubtract::~SgAsmBinarySubtract() {
    destructorHelper();
}

SgAsmBinarySubtract::SgAsmBinarySubtract() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinarySubtract::SgAsmBinarySubtract(SgAsmExpression* const& lhs,
                                         SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinarySubtract::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
