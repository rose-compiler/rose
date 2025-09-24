//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeVisibleAnnotations            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeVisibleAnnotations_IMPL
#include <SgAsmJvmRuntimeVisibleAnnotations.h>

std::vector<SgAsmJvmRuntimeAnnotation*> const&
SgAsmJvmRuntimeVisibleAnnotations::get_annotations() const {
    return p_annotations;
}

std::vector<SgAsmJvmRuntimeAnnotation*>&
SgAsmJvmRuntimeVisibleAnnotations::get_annotations() {
    return p_annotations;
}

SgAsmJvmRuntimeVisibleAnnotations::~SgAsmJvmRuntimeVisibleAnnotations() {
    destructorHelper();
}

SgAsmJvmRuntimeVisibleAnnotations::SgAsmJvmRuntimeVisibleAnnotations() {}

void
SgAsmJvmRuntimeVisibleAnnotations::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
