//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmIndexedAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmIndexedAttribute_IMPL
#include <SgAsmJvmIndexedAttribute.h>

unsigned const&
SgAsmJvmIndexedAttribute::get_attribute_type() const {
    return p_attribute_type;
}

void
SgAsmJvmIndexedAttribute::set_attribute_type(unsigned const& x) {
    this->p_attribute_type = x;
    set_isModified(true);
}

SgUnsigned16List const&
SgAsmJvmIndexedAttribute::get_indices() const {
    return p_indices;
}

void
SgAsmJvmIndexedAttribute::set_indices(SgUnsigned16List const& x) {
    this->p_indices = x;
    set_isModified(true);
}

SgAsmJvmIndexedAttribute::~SgAsmJvmIndexedAttribute() {
    destructorHelper();
}

SgAsmJvmIndexedAttribute::SgAsmJvmIndexedAttribute()
    : p_attribute_type(SgAsmJvmIndexedAttribute::ATTR_NONE) {}

void
SgAsmJvmIndexedAttribute::initializeProperties() {
    p_attribute_type = SgAsmJvmIndexedAttribute::ATTR_NONE;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
