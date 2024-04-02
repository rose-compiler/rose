//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmAttribute_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmAttribute::get_attribute_name_index() const {
    return p_attribute_name_index;
}

void
SgAsmJvmAttribute::set_attribute_name_index(uint16_t const& x) {
    this->p_attribute_name_index = x;
    set_isModified(true);
}

uint32_t const&
SgAsmJvmAttribute::get_attribute_length() const {
    return p_attribute_length;
}

void
SgAsmJvmAttribute::set_attribute_length(uint32_t const& x) {
    this->p_attribute_length = x;
    set_isModified(true);
}

SgAsmJvmAttribute::~SgAsmJvmAttribute() {
    destructorHelper();
}

SgAsmJvmAttribute::SgAsmJvmAttribute()
    : p_attribute_name_index(0)
    , p_attribute_length(0) {}

void
SgAsmJvmAttribute::initializeProperties() {
    p_attribute_name_index = 0;
    p_attribute_length = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
