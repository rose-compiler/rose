//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethodParametersEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmMethodParametersEntry_IMPL
#include <SgAsmJvmMethodParametersEntry.h>

uint16_t const&
SgAsmJvmMethodParametersEntry::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmMethodParametersEntry::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmMethodParametersEntry::get_access_flags() const {
    return p_access_flags;
}

void
SgAsmJvmMethodParametersEntry::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

SgAsmJvmMethodParametersEntry::~SgAsmJvmMethodParametersEntry() {
    destructorHelper();
}

SgAsmJvmMethodParametersEntry::SgAsmJvmMethodParametersEntry()
    : p_name_index(0)
    , p_access_flags(0) {}

void
SgAsmJvmMethodParametersEntry::initializeProperties() {
    p_name_index = 0;
    p_access_flags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
