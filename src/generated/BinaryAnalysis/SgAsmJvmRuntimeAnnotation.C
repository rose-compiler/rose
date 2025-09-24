//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeAnnotation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeAnnotation_IMPL
#include <SgAsmJvmRuntimeAnnotation.h>

uint16_t const&
SgAsmJvmRuntimeAnnotation::get_type_index() const {
    return p_type_index;
}

void
SgAsmJvmRuntimeAnnotation::set_type_index(uint16_t const& x) {
    this->p_type_index = x;
    set_isModified(true);
}

std::vector<SgAsmJvmRuntimeAnnotationPair*> const&
SgAsmJvmRuntimeAnnotation::get_element_value_pairs() const {
    return p_element_value_pairs;
}

std::vector<SgAsmJvmRuntimeAnnotationPair*>&
SgAsmJvmRuntimeAnnotation::get_element_value_pairs() {
    return p_element_value_pairs;
}

SgAsmJvmRuntimeAnnotation::~SgAsmJvmRuntimeAnnotation() {
    destructorHelper();
}

SgAsmJvmRuntimeAnnotation::SgAsmJvmRuntimeAnnotation()
    : p_type_index(0) {}

void
SgAsmJvmRuntimeAnnotation::initializeProperties() {
    p_type_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
