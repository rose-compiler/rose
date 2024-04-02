//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmBootstrapMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmBootstrapMethod_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmBootstrapMethod::get_bootstrap_method_ref() const {
    return p_bootstrap_method_ref;
}

void
SgAsmJvmBootstrapMethod::set_bootstrap_method_ref(uint16_t const& x) {
    this->p_bootstrap_method_ref = x;
    set_isModified(true);
}

SgUnsigned16List const&
SgAsmJvmBootstrapMethod::get_bootstrap_arguments() const {
    return p_bootstrap_arguments;
}

void
SgAsmJvmBootstrapMethod::set_bootstrap_arguments(SgUnsigned16List const& x) {
    this->p_bootstrap_arguments = x;
    set_isModified(true);
}

SgAsmJvmBootstrapMethod::~SgAsmJvmBootstrapMethod() {
    destructorHelper();
}

SgAsmJvmBootstrapMethod::SgAsmJvmBootstrapMethod()
    : p_bootstrap_method_ref(0) {}

void
SgAsmJvmBootstrapMethod::initializeProperties() {
    p_bootstrap_method_ref = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
