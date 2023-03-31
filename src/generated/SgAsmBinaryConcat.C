//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryConcat                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
