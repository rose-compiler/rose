//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeVisibilityParamAnnotations            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeVisibilityParamAnnotations_IMPL
#include <SgAsmJvmRuntimeVisibilityParamAnnotations.h>

std::vector<SgAsmJvmRuntimeParameterAnnotation*> const&
SgAsmJvmRuntimeVisibilityParamAnnotations::get_parameter_annotations() const {
    return p_parameter_annotations;
}

std::vector<SgAsmJvmRuntimeParameterAnnotation*>&
SgAsmJvmRuntimeVisibilityParamAnnotations::get_parameter_annotations() {
    return p_parameter_annotations;
}

bool const&
SgAsmJvmRuntimeVisibilityParamAnnotations::get_isVisible() const {
    return p_isVisible;
}

void
SgAsmJvmRuntimeVisibilityParamAnnotations::set_isVisible(bool const& x) {
    this->p_isVisible = x;
    set_isModified(true);
}

SgAsmJvmRuntimeVisibilityParamAnnotations::~SgAsmJvmRuntimeVisibilityParamAnnotations() {
    destructorHelper();
}

SgAsmJvmRuntimeVisibilityParamAnnotations::SgAsmJvmRuntimeVisibilityParamAnnotations()
    : p_isVisible(true) {}

void
SgAsmJvmRuntimeVisibilityParamAnnotations::initializeProperties() {
    p_isVisible = true;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
