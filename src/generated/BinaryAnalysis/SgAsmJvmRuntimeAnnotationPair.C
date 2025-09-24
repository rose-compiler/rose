//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeAnnotationPair            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeAnnotationPair_IMPL
#include <SgAsmJvmRuntimeAnnotationPair.h>

uint16_t const&
SgAsmJvmRuntimeAnnotationPair::get_element_name_index() const {
    return p_element_name_index;
}

void
SgAsmJvmRuntimeAnnotationPair::set_element_name_index(uint16_t const& x) {
    this->p_element_name_index = x;
    set_isModified(true);
}

SgAsmJvmRuntimeAnnotationValue* const&
SgAsmJvmRuntimeAnnotationPair::get_value() const {
    return p_value;
}

void
SgAsmJvmRuntimeAnnotationPair::set_value(SgAsmJvmRuntimeAnnotationValue* const& x) {
    this->p_value = x;
    set_isModified(true);
}

SgAsmJvmRuntimeAnnotationPair::~SgAsmJvmRuntimeAnnotationPair() {
    destructorHelper();
}

SgAsmJvmRuntimeAnnotationPair::SgAsmJvmRuntimeAnnotationPair()
    : p_element_name_index(0)
    , p_value(nullptr) {}

void
SgAsmJvmRuntimeAnnotationPair::initializeProperties() {
    p_element_name_index = 0;
    p_value = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
