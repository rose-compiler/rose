//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeVisibleParameterAnnotations            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeVisibleParameterAnnotations_IMPL
#include <SgAsmJvmRuntimeVisibleParameterAnnotations.h>

std::vector<SgAsmJvmRuntimeParameterAnnotation*> const&
SgAsmJvmRuntimeVisibleParameterAnnotations::get_parameter_annotations() const {
    return p_parameter_annotations;
}

std::vector<SgAsmJvmRuntimeParameterAnnotation*>&
SgAsmJvmRuntimeVisibleParameterAnnotations::get_parameter_annotations() {
    return p_parameter_annotations;
}

SgAsmJvmRuntimeVisibleParameterAnnotations::~SgAsmJvmRuntimeVisibleParameterAnnotations() {
    destructorHelper();
}

SgAsmJvmRuntimeVisibleParameterAnnotations::SgAsmJvmRuntimeVisibleParameterAnnotations() {}

void
SgAsmJvmRuntimeVisibleParameterAnnotations::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
