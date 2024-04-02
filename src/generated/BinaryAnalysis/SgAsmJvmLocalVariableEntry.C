//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLocalVariableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLocalVariableEntry_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmLocalVariableEntry::get_start_pc() const {
    return p_start_pc;
}

void
SgAsmJvmLocalVariableEntry::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableEntry::get_length() const {
    return p_length;
}

void
SgAsmJvmLocalVariableEntry::set_length(uint16_t const& x) {
    this->p_length = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableEntry::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmLocalVariableEntry::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableEntry::get_descriptor_index() const {
    return p_descriptor_index;
}

void
SgAsmJvmLocalVariableEntry::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLocalVariableEntry::get_index() const {
    return p_index;
}

void
SgAsmJvmLocalVariableEntry::set_index(uint16_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

SgAsmJvmLocalVariableEntry::~SgAsmJvmLocalVariableEntry() {
    destructorHelper();
}

SgAsmJvmLocalVariableEntry::SgAsmJvmLocalVariableEntry()
    : p_start_pc(0)
    , p_length(0)
    , p_name_index(0)
    , p_descriptor_index(0)
    , p_index(0) {}

void
SgAsmJvmLocalVariableEntry::initializeProperties() {
    p_start_pc = 0;
    p_length = 0;
    p_name_index = 0;
    p_descriptor_index = 0;
    p_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
