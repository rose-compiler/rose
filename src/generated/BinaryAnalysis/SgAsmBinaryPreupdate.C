//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryPreupdate            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryPreupdate_IMPL
#include <SgAsmBinaryPreupdate.h>

SgAsmBinaryPreupdate::~SgAsmBinaryPreupdate() {
    destructorHelper();
}

SgAsmBinaryPreupdate::SgAsmBinaryPreupdate() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryPreupdate::SgAsmBinaryPreupdate(SgAsmExpression* const& lhs,
                                           SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryPreupdate::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
