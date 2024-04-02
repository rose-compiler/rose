//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSignature            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmSignature_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmSignature::get_signature_index() const {
    return p_signature_index;
}

void
SgAsmJvmSignature::set_signature_index(uint16_t const& x) {
    this->p_signature_index = x;
    set_isModified(true);
}

SgAsmJvmSignature::~SgAsmJvmSignature() {
    destructorHelper();
}

SgAsmJvmSignature::SgAsmJvmSignature()
    : p_signature_index(0) {}

void
SgAsmJvmSignature::initializeProperties() {
    p_signature_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
