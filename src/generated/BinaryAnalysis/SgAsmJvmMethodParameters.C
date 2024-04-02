//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethodParameters            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmMethodParameters_IMPL
#include <SgAsmJvmMethodParameters.h>

SgAsmJvmMethodParametersEntryPtrList const&
SgAsmJvmMethodParameters::get_parameters() const {
    return p_parameters;
}

SgAsmJvmMethodParametersEntryPtrList&
SgAsmJvmMethodParameters::get_parameters() {
    return p_parameters;
}

void
SgAsmJvmMethodParameters::set_parameters(SgAsmJvmMethodParametersEntryPtrList const& x) {
    this->p_parameters = x;
    set_isModified(true);
}

SgAsmJvmMethodParameters::~SgAsmJvmMethodParameters() {
    destructorHelper();
}

SgAsmJvmMethodParameters::SgAsmJvmMethodParameters() {}

void
SgAsmJvmMethodParameters::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
