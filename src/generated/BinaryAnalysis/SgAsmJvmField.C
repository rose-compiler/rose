//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmField            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmField_IMPL
#include <SgAsmJvmField.h>

uint16_t const&
SgAsmJvmField::get_access_flags() const {
    return p_access_flags;
}

void
SgAsmJvmField::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmField::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmField::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmField::get_descriptor_index() const {
    return p_descriptor_index;
}

void
SgAsmJvmField::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

SgAsmJvmAttributeTable* const&
SgAsmJvmField::get_attribute_table() const {
    return p_attribute_table;
}

void
SgAsmJvmField::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    changeChildPointer(this->p_attribute_table, const_cast<SgAsmJvmAttributeTable*&>(x));
    set_isModified(true);
}

SgAsmJvmField::~SgAsmJvmField() {
    destructorHelper();
}

SgAsmJvmField::SgAsmJvmField()
    : p_access_flags(0)
    , p_name_index(0)
    , p_descriptor_index(0)
    , p_attribute_table(nullptr) {}

void
SgAsmJvmField::initializeProperties() {
    p_access_flags = 0;
    p_name_index = 0;
    p_descriptor_index = 0;
    p_attribute_table = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
