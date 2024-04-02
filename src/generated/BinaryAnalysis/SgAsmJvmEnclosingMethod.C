//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmEnclosingMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmEnclosingMethod_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmEnclosingMethod::get_class_index() const {
    return p_class_index;
}

void
SgAsmJvmEnclosingMethod::set_class_index(uint16_t const& x) {
    this->p_class_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmEnclosingMethod::get_method_index() const {
    return p_method_index;
}

void
SgAsmJvmEnclosingMethod::set_method_index(uint16_t const& x) {
    this->p_method_index = x;
    set_isModified(true);
}

SgAsmJvmEnclosingMethod::~SgAsmJvmEnclosingMethod() {
    destructorHelper();
}

SgAsmJvmEnclosingMethod::SgAsmJvmEnclosingMethod()
    : p_class_index(0)
    , p_method_index(0) {}

void
SgAsmJvmEnclosingMethod::initializeProperties() {
    p_class_index = 0;
    p_method_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
