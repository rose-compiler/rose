//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeInvisibleAnnotations            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeInvisibleAnnotations_IMPL
#include <SgAsmJvmRuntimeInvisibleAnnotations.h>

std::vector<SgAsmJvmRuntimeAnnotation*> const&
SgAsmJvmRuntimeInvisibleAnnotations::get_annotations() const {
    return p_annotations;
}

std::vector<SgAsmJvmRuntimeAnnotation*>&
SgAsmJvmRuntimeInvisibleAnnotations::get_annotations() {
    return p_annotations;
}

SgAsmJvmRuntimeInvisibleAnnotations::~SgAsmJvmRuntimeInvisibleAnnotations() {
    destructorHelper();
}

SgAsmJvmRuntimeInvisibleAnnotations::SgAsmJvmRuntimeInvisibleAnnotations() {}

void
SgAsmJvmRuntimeInvisibleAnnotations::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
