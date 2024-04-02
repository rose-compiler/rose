//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryLsl            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryLsl_IMPL
#include <sage3basic.h>

SgAsmBinaryLsl::~SgAsmBinaryLsl() {
    destructorHelper();
}

SgAsmBinaryLsl::SgAsmBinaryLsl() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryLsl::SgAsmBinaryLsl(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryLsl::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
