//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLocalVariableTypeEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLocalVariableTypeEntry_IMPL
#include <SgAsmJvmLocalVariableTypeEntry.h>

uint16_t const&
SgAsmJvmLocalVariableTypeEntry::get_start_pc() const {
    return p_start_pc;
}

void
SgAsmJvmLocalVariableTypeEntry::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableTypeEntry::get_length() const {
    return p_length;
}

void
SgAsmJvmLocalVariableTypeEntry::set_length(uint16_t const& x) {
    this->p_length = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableTypeEntry::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmLocalVariableTypeEntry::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableTypeEntry::get_signature_index() const {
    return p_signature_index;
}

void
SgAsmJvmLocalVariableTypeEntry::set_signature_index(uint16_t const& x) {
    this->p_signature_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableTypeEntry::get_index() const {
    return p_index;
}

void
SgAsmJvmLocalVariableTypeEntry::set_index(uint16_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

SgAsmJvmLocalVariableTypeEntry::~SgAsmJvmLocalVariableTypeEntry() {
    destructorHelper();
}

SgAsmJvmLocalVariableTypeEntry::SgAsmJvmLocalVariableTypeEntry()
    : p_start_pc(0)
    , p_length(0)
    , p_name_index(0)
    , p_signature_index(0)
    , p_index(0) {}

void
SgAsmJvmLocalVariableTypeEntry::initializeProperties() {
    p_start_pc = 0;
    p_length = 0;
    p_name_index = 0;
    p_signature_index = 0;
    p_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
