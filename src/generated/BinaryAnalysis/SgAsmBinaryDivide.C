//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryDivide            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryDivide_IMPL
#include <sage3basic.h>

SgAsmBinaryDivide::~SgAsmBinaryDivide() {
    destructorHelper();
}

SgAsmBinaryDivide::SgAsmBinaryDivide() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryDivide::SgAsmBinaryDivide(SgAsmExpression* const& lhs,
                                     SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryDivide::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
