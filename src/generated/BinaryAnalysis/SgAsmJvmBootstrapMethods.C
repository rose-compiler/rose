//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmBootstrapMethods            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmBootstrapMethods_IMPL
#include <sage3basic.h>

SgAsmJvmBootstrapMethodPtrList const&
SgAsmJvmBootstrapMethods::get_bootstrap_methods() const {
    return p_bootstrap_methods;
}

SgAsmJvmBootstrapMethodPtrList&
SgAsmJvmBootstrapMethods::get_bootstrap_methods() {
    return p_bootstrap_methods;
}

void
SgAsmJvmBootstrapMethods::set_bootstrap_methods(SgAsmJvmBootstrapMethodPtrList const& x) {
    this->p_bootstrap_methods = x;
    set_isModified(true);
}

SgAsmJvmBootstrapMethods::~SgAsmJvmBootstrapMethods() {
    destructorHelper();
}

SgAsmJvmBootstrapMethods::SgAsmJvmBootstrapMethods() {}

void
SgAsmJvmBootstrapMethods::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
