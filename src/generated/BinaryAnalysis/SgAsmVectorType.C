//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmVectorType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmVectorType_IMPL
#include <SgAsmVectorType.h>

SgAsmVectorType::~SgAsmVectorType() {
    destructorHelper();
}

SgAsmVectorType::SgAsmVectorType()
    : p_nElmts(0)
    , p_elmtType(nullptr) {}

void
SgAsmVectorType::initializeProperties() {
    p_nElmts = 0;
    p_elmtType = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
