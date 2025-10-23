//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmTableAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmTableAttribute_IMPL
#include <SgAsmJvmTableAttribute.h>

unsigned const&
SgAsmJvmTableAttribute::get_attribute_type() const {
    return p_attribute_type;
}

void
SgAsmJvmTableAttribute::set_attribute_type(unsigned const& x) {
    this->p_attribute_type = x;
    set_isModified(true);
}

SgUnsigned16List const&
SgAsmJvmTableAttribute::get_table() const {
    return p_table;
}

void
SgAsmJvmTableAttribute::set_table(SgUnsigned16List const& x) {
    this->p_table = x;
    set_isModified(true);
}

SgAsmJvmTableAttribute::~SgAsmJvmTableAttribute() {
    destructorHelper();
}

SgAsmJvmTableAttribute::SgAsmJvmTableAttribute()
    : p_attribute_type(SgAsmJvmTableAttribute::ATTR_NONE) {}

void
SgAsmJvmTableAttribute::initializeProperties() {
    p_attribute_type = SgAsmJvmTableAttribute::ATTR_NONE;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
