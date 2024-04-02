//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantPoolEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmConstantPoolEntry_IMPL
#include <sage3basic.h>

SgAsmJvmConstantPoolEntry::Kind const&
SgAsmJvmConstantPoolEntry::get_tag() const {
    return p_tag;
}

void
SgAsmJvmConstantPoolEntry::set_tag(SgAsmJvmConstantPoolEntry::Kind const& x) {
    this->p_tag = x;
    set_isModified(true);
}

uint32_t const&
SgAsmJvmConstantPoolEntry::get_bytes() const {
    return p_bytes;
}

void
SgAsmJvmConstantPoolEntry::set_bytes(uint32_t const& x) {
    this->p_bytes = x;
    set_isModified(true);
}

uint32_t const&
SgAsmJvmConstantPoolEntry::get_hi_bytes() const {
    return p_hi_bytes;
}

void
SgAsmJvmConstantPoolEntry::set_hi_bytes(uint32_t const& x) {
    this->p_hi_bytes = x;
    set_isModified(true);
}

uint32_t const&
SgAsmJvmConstantPoolEntry::get_low_bytes() const {
    return p_low_bytes;
}

void
SgAsmJvmConstantPoolEntry::set_low_bytes(uint32_t const& x) {
    this->p_low_bytes = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_bootstrap_method_attr_index() const {
    return p_bootstrap_method_attr_index;
}

void
SgAsmJvmConstantPoolEntry::set_bootstrap_method_attr_index(uint16_t const& x) {
    this->p_bootstrap_method_attr_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_class_index() const {
    return p_class_index;
}

void
SgAsmJvmConstantPoolEntry::set_class_index(uint16_t const& x) {
    this->p_class_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_descriptor_index() const {
    return p_descriptor_index;
}

void
SgAsmJvmConstantPoolEntry::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmConstantPoolEntry::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_name_and_type_index() const {
    return p_name_and_type_index;
}

void
SgAsmJvmConstantPoolEntry::set_name_and_type_index(uint16_t const& x) {
    this->p_name_and_type_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_reference_index() const {
    return p_reference_index;
}

void
SgAsmJvmConstantPoolEntry::set_reference_index(uint16_t const& x) {
    this->p_reference_index = x;
    set_isModified(true);
}

uint8_t const&
SgAsmJvmConstantPoolEntry::get_reference_kind() const {
    return p_reference_kind;
}

void
SgAsmJvmConstantPoolEntry::set_reference_kind(uint8_t const& x) {
    this->p_reference_kind = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_string_index() const {
    return p_string_index;
}

void
SgAsmJvmConstantPoolEntry::set_string_index(uint16_t const& x) {
    this->p_string_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmConstantPoolEntry::get_length() const {
    return p_length;
}

void
SgAsmJvmConstantPoolEntry::set_length(uint16_t const& x) {
    this->p_length = x;
    set_isModified(true);
}

char* const&
SgAsmJvmConstantPoolEntry::get_utf8_bytes() const {
    return p_utf8_bytes;
}

void
SgAsmJvmConstantPoolEntry::set_utf8_bytes(char* const& x) {
    this->p_utf8_bytes = x;
    set_isModified(true);
}

SgAsmJvmConstantPoolEntry::~SgAsmJvmConstantPoolEntry() {
    destructorHelper();
}

SgAsmJvmConstantPoolEntry::SgAsmJvmConstantPoolEntry()
    : p_tag(SgAsmJvmConstantPoolEntry::EMPTY)
    , p_bytes(0)
    , p_hi_bytes(0)
    , p_low_bytes(0)
    , p_bootstrap_method_attr_index(0)
    , p_class_index(0)
    , p_descriptor_index(0)
    , p_name_index(0)
    , p_name_and_type_index(0)
    , p_reference_index(0)
    , p_reference_kind(0)
    , p_string_index(0)
    , p_length(0)
    , p_utf8_bytes(nullptr) {}

void
SgAsmJvmConstantPoolEntry::initializeProperties() {
    p_tag = SgAsmJvmConstantPoolEntry::EMPTY;
    p_bytes = 0;
    p_hi_bytes = 0;
    p_low_bytes = 0;
    p_bootstrap_method_attr_index = 0;
    p_class_index = 0;
    p_descriptor_index = 0;
    p_name_index = 0;
    p_name_and_type_index = 0;
    p_reference_index = 0;
    p_reference_kind = 0;
    p_string_index = 0;
    p_length = 0;
    p_utf8_bytes = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
