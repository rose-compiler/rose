//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAnnotationDefault            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmAnnotationDefault_IMPL
#include <SgAsmJvmAnnotationDefault.h>

SgAsmJvmRuntimeAnnotationValue* const&
SgAsmJvmAnnotationDefault::get_default_value() const {
    return p_default_value;
}

void
SgAsmJvmAnnotationDefault::set_default_value(SgAsmJvmRuntimeAnnotationValue* const& x) {
    this->p_default_value = x;
    set_isModified(true);
}

SgAsmJvmAnnotationDefault::~SgAsmJvmAnnotationDefault() {
    destructorHelper();
}

SgAsmJvmAnnotationDefault::SgAsmJvmAnnotationDefault()
    : p_default_value(nullptr) {}

void
SgAsmJvmAnnotationDefault::initializeProperties() {
    p_default_value = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
