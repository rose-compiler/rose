//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmCodeAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmCodeAttribute_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmCodeAttribute::get_max_stack() const {
    return p_max_stack;
}

void
SgAsmJvmCodeAttribute::set_max_stack(uint16_t const& x) {
    this->p_max_stack = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmCodeAttribute::get_max_locals() const {
    return p_max_locals;
}

void
SgAsmJvmCodeAttribute::set_max_locals(uint16_t const& x) {
    this->p_max_locals = x;
    set_isModified(true);
}

uint32_t const&
SgAsmJvmCodeAttribute::get_code_length() const {
    return p_code_length;
}

void
SgAsmJvmCodeAttribute::set_code_length(uint32_t const& x) {
    this->p_code_length = x;
    set_isModified(true);
}

const char* const&
SgAsmJvmCodeAttribute::get_code() const {
    return p_code;
}

void
SgAsmJvmCodeAttribute::set_code(const char* const& x) {
    this->p_code = x;
    set_isModified(true);
}

SgAsmJvmExceptionTable* const&
SgAsmJvmCodeAttribute::get_exception_table() const {
    return p_exception_table;
}

void
SgAsmJvmCodeAttribute::set_exception_table(SgAsmJvmExceptionTable* const& x) {
    this->p_exception_table = x;
    set_isModified(true);
}

SgAsmJvmAttributeTable* const&
SgAsmJvmCodeAttribute::get_attribute_table() const {
    return p_attribute_table;
}

void
SgAsmJvmCodeAttribute::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmJvmCodeAttribute::get_code_offset() const {
    return p_code_offset;
}

void
SgAsmJvmCodeAttribute::set_code_offset(rose_addr_t const& x) {
    this->p_code_offset = x;
    set_isModified(true);
}

SgAsmJvmCodeAttribute::~SgAsmJvmCodeAttribute() {
    destructorHelper();
}

SgAsmJvmCodeAttribute::SgAsmJvmCodeAttribute()
    : p_max_stack(0)
    , p_max_locals(0)
    , p_code_length(0)
    , p_code(nullptr)
    , p_exception_table(nullptr)
    , p_attribute_table(nullptr)
    , p_code_offset(0) {}

void
SgAsmJvmCodeAttribute::initializeProperties() {
    p_max_stack = 0;
    p_max_locals = 0;
    p_code_length = 0;
    p_code = nullptr;
    p_exception_table = nullptr;
    p_attribute_table = nullptr;
    p_code_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
