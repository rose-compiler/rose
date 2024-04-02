//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodSpec            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodSpec_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilMethodSpec::get_Method() const {
    return p_Method;
}

void
SgAsmCilMethodSpec::set_Method(uint32_t const& x) {
    this->p_Method = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodSpec::get_Instantiation() const {
    return p_Instantiation;
}

void
SgAsmCilMethodSpec::set_Instantiation(uint32_t const& x) {
    this->p_Instantiation = x;
    set_isModified(true);
}

SgAsmCilMethodSpec::~SgAsmCilMethodSpec() {
    destructorHelper();
}

SgAsmCilMethodSpec::SgAsmCilMethodSpec()
    : p_Method(0)
    , p_Instantiation(0) {}

void
SgAsmCilMethodSpec::initializeProperties() {
    p_Method = 0;
    p_Instantiation = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
