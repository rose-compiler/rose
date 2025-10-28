//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmIndexedAttr            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmIndexedAttr_IMPL
#include <SgAsmJvmIndexedAttr.h>

unsigned const&
SgAsmJvmIndexedAttr::get_attribute_type() const {
    return p_attribute_type;
}

void
SgAsmJvmIndexedAttr::set_attribute_type(unsigned const& x) {
    this->p_attribute_type = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmIndexedAttr::get_index() const {
    return p_index;
}

void
SgAsmJvmIndexedAttr::set_index(uint16_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

SgAsmJvmIndexedAttr::~SgAsmJvmIndexedAttr() {
    destructorHelper();
}

SgAsmJvmIndexedAttr::SgAsmJvmIndexedAttr()
    : p_attribute_type(SgAsmJvmIndexedAttr::ATTR_NONE) {}

void
SgAsmJvmIndexedAttr::initializeProperties() {
    p_attribute_type = SgAsmJvmIndexedAttr::ATTR_NONE;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
