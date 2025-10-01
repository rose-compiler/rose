//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeParameterAnnotation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeParameterAnnotation_IMPL
#include <SgAsmJvmRuntimeParameterAnnotation.h>

std::vector<SgAsmJvmRuntimeAnnotation*> const&
SgAsmJvmRuntimeParameterAnnotation::get_annotations() const {
    return p_annotations;
}

std::vector<SgAsmJvmRuntimeAnnotation*>&
SgAsmJvmRuntimeParameterAnnotation::get_annotations() {
    return p_annotations;
}

SgAsmJvmRuntimeParameterAnnotation::~SgAsmJvmRuntimeParameterAnnotation() {
    destructorHelper();
}

SgAsmJvmRuntimeParameterAnnotation::SgAsmJvmRuntimeParameterAnnotation() {}

void
SgAsmJvmRuntimeParameterAnnotation::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
