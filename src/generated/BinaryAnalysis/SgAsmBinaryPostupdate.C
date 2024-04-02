//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryPostupdate            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryPostupdate_IMPL
#include <sage3basic.h>

SgAsmBinaryPostupdate::~SgAsmBinaryPostupdate() {
    destructorHelper();
}

SgAsmBinaryPostupdate::SgAsmBinaryPostupdate() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryPostupdate::SgAsmBinaryPostupdate(SgAsmExpression* const& lhs,
                                             SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryPostupdate::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
