//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryConcat            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryConcat_IMPL
#include <sage3basic.h>

SgAsmBinaryConcat::~SgAsmBinaryConcat() {
    destructorHelper();
}

SgAsmBinaryConcat::SgAsmBinaryConcat() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryConcat::SgAsmBinaryConcat(SgAsmExpression* const& lhs,
                                     SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryConcat::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
