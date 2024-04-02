//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmFileHeader_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmFileHeader::get_minor_version() const {
    return p_minor_version;
}

void
SgAsmJvmFileHeader::set_minor_version(uint16_t const& x) {
    this->p_minor_version = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmFileHeader::get_major_version() const {
    return p_major_version;
}

void
SgAsmJvmFileHeader::set_major_version(uint16_t const& x) {
    this->p_major_version = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmFileHeader::get_access_flags() const {
    return p_access_flags;
}

void
SgAsmJvmFileHeader::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmFileHeader::get_this_class() const {
    return p_this_class;
}

void
SgAsmJvmFileHeader::set_this_class(uint16_t const& x) {
    this->p_this_class = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmFileHeader::get_super_class() const {
    return p_super_class;
}

void
SgAsmJvmFileHeader::set_super_class(uint16_t const& x) {
    this->p_super_class = x;
    set_isModified(true);
}

SgAsmJvmConstantPool* const&
SgAsmJvmFileHeader::get_constant_pool() const {
    return p_constant_pool;
}

void
SgAsmJvmFileHeader::set_constant_pool(SgAsmJvmConstantPool* const& x) {
    this->p_constant_pool = x;
    set_isModified(true);
}

std::list<uint16_t> const&
SgAsmJvmFileHeader::get_interfaces() const {
    return p_interfaces;
}

std::list<uint16_t>&
SgAsmJvmFileHeader::get_interfaces() {
    return p_interfaces;
}

void
SgAsmJvmFileHeader::set_interfaces(std::list<uint16_t> const& x) {
    this->p_interfaces = x;
    set_isModified(true);
}

SgAsmJvmFieldTable* const&
SgAsmJvmFileHeader::get_field_table() const {
    return p_field_table;
}

void
SgAsmJvmFileHeader::set_field_table(SgAsmJvmFieldTable* const& x) {
    this->p_field_table = x;
    set_isModified(true);
}

SgAsmJvmMethodTable* const&
SgAsmJvmFileHeader::get_method_table() const {
    return p_method_table;
}

void
SgAsmJvmFileHeader::set_method_table(SgAsmJvmMethodTable* const& x) {
    this->p_method_table = x;
    set_isModified(true);
}

SgAsmJvmAttributeTable* const&
SgAsmJvmFileHeader::get_attribute_table() const {
    return p_attribute_table;
}

void
SgAsmJvmFileHeader::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

SgAsmJvmFileHeader::~SgAsmJvmFileHeader() {
    destructorHelper();
}

SgAsmJvmFileHeader::SgAsmJvmFileHeader()
    : p_minor_version(0)
    , p_major_version(0)
    , p_access_flags(0)
    , p_this_class(0)
    , p_super_class(0)
    , p_constant_pool(nullptr)
    , p_field_table(nullptr)
    , p_method_table(nullptr)
    , p_attribute_table(nullptr) {}

void
SgAsmJvmFileHeader::initializeProperties() {
    p_minor_version = 0;
    p_major_version = 0;
    p_access_flags = 0;
    p_this_class = 0;
    p_super_class = 0;
    p_constant_pool = nullptr;
    p_field_table = nullptr;
    p_method_table = nullptr;
    p_attribute_table = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
