//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLineNumberEntry_IMPL
#include <SgAsmJvmLineNumberEntry.h>

uint16_t const&
SgAsmJvmLineNumberEntry::get_start_pc() const {
    return p_start_pc;
}

void
SgAsmJvmLineNumberEntry::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmLineNumberEntry::get_line_number() const {
    return p_line_number;
}

void
SgAsmJvmLineNumberEntry::set_line_number(uint16_t const& x) {
    this->p_line_number = x;
    set_isModified(true);
}

SgAsmJvmLineNumberEntry::~SgAsmJvmLineNumberEntry() {
    destructorHelper();
}

SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry()
    : p_start_pc(0)
    , p_line_number(0) {}

void
SgAsmJvmLineNumberEntry::initializeProperties() {
    p_start_pc = 0;
    p_line_number = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
