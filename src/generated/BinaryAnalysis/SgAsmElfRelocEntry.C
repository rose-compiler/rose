//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfRelocEntry_IMPL
#include <sage3basic.h>

rose_addr_t const&
SgAsmElfRelocEntry::get_r_offset() const {
    return p_r_offset;
}

void
SgAsmElfRelocEntry::set_r_offset(rose_addr_t const& x) {
    this->p_r_offset = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfRelocEntry::get_r_addend() const {
    return p_r_addend;
}

void
SgAsmElfRelocEntry::set_r_addend(rose_addr_t const& x) {
    this->p_r_addend = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfRelocEntry::get_sym() const {
    return p_sym;
}

void
SgAsmElfRelocEntry::set_sym(unsigned long const& x) {
    this->p_sym = x;
    set_isModified(true);
}

SgAsmElfRelocEntry::RelocType const&
SgAsmElfRelocEntry::get_type() const {
    return p_type;
}

void
SgAsmElfRelocEntry::set_type(SgAsmElfRelocEntry::RelocType const& x) {
    this->p_type = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfRelocEntry::get_extra() const {
    return p_extra;
}

SgUnsignedCharList&
SgAsmElfRelocEntry::get_extra() {
    return p_extra;
}

void
SgAsmElfRelocEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

SgAsmElfRelocEntry::~SgAsmElfRelocEntry() {
    destructorHelper();
}

SgAsmElfRelocEntry::SgAsmElfRelocEntry()
    : p_r_offset(0)
    , p_r_addend(0)
    , p_sym(0)
    , p_type(R_386_NONE) {}

void
SgAsmElfRelocEntry::initializeProperties() {
    p_r_offset = 0;
    p_r_addend = 0;
    p_sym = 0;
    p_type = R_386_NONE;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
