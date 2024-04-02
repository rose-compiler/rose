//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilTypeSpec            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilTypeSpec_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilTypeSpec::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilTypeSpec::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

SgAsmCilTypeSpec::~SgAsmCilTypeSpec() {
    destructorHelper();
}

SgAsmCilTypeSpec::SgAsmCilTypeSpec()
    : p_Signature(0) {}

void
SgAsmCilTypeSpec::initializeProperties() {
    p_Signature = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
