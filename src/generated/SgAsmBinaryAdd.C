//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryAdd            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmBinaryAdd::~SgAsmBinaryAdd() {
    destructorHelper();
}

SgAsmBinaryAdd::SgAsmBinaryAdd() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryAdd::SgAsmBinaryAdd(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryAdd::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
