//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilFieldRVA            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilFieldRVA_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilFieldRVA::get_RVA() const {
    return p_RVA;
}

void
SgAsmCilFieldRVA::set_RVA(uint32_t const& x) {
    this->p_RVA = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilFieldRVA::get_Field() const {
    return p_Field;
}

void
SgAsmCilFieldRVA::set_Field(uint32_t const& x) {
    this->p_Field = x;
    set_isModified(true);
}

SgAsmCilFieldRVA::~SgAsmCilFieldRVA() {
    destructorHelper();
}

SgAsmCilFieldRVA::SgAsmCilFieldRVA()
    : p_RVA(0)
    , p_Field(0) {}

void
SgAsmCilFieldRVA::initializeProperties() {
    p_RVA = 0;
    p_Field = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
