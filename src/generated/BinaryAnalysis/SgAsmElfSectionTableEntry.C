//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSectionTableEntry_IMPL
#include <sage3basic.h>

unsigned const&
SgAsmElfSectionTableEntry::get_sh_name() const {
    return p_sh_name;
}

void
SgAsmElfSectionTableEntry::set_sh_name(unsigned const& x) {
    this->p_sh_name = x;
    set_isModified(true);
}

SgAsmElfSectionTableEntry::SectionType const&
SgAsmElfSectionTableEntry::get_sh_type() const {
    return p_sh_type;
}

void
SgAsmElfSectionTableEntry::set_sh_type(SgAsmElfSectionTableEntry::SectionType const& x) {
    this->p_sh_type = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfSectionTableEntry::get_sh_link() const {
    return p_sh_link;
}

void
SgAsmElfSectionTableEntry::set_sh_link(unsigned long const& x) {
    this->p_sh_link = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfSectionTableEntry::get_sh_info() const {
    return p_sh_info;
}

void
SgAsmElfSectionTableEntry::set_sh_info(unsigned long const& x) {
    this->p_sh_info = x;
    set_isModified(true);
}

uint64_t const&
SgAsmElfSectionTableEntry::get_sh_flags() const {
    return p_sh_flags;
}

void
SgAsmElfSectionTableEntry::set_sh_flags(uint64_t const& x) {
    this->p_sh_flags = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addr() const {
    return p_sh_addr;
}

void
SgAsmElfSectionTableEntry::set_sh_addr(rose_addr_t const& x) {
    this->p_sh_addr = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_offset() const {
    return p_sh_offset;
}

void
SgAsmElfSectionTableEntry::set_sh_offset(rose_addr_t const& x) {
    this->p_sh_offset = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_size() const {
    return p_sh_size;
}

void
SgAsmElfSectionTableEntry::set_sh_size(rose_addr_t const& x) {
    this->p_sh_size = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addralign() const {
    return p_sh_addralign;
}

void
SgAsmElfSectionTableEntry::set_sh_addralign(rose_addr_t const& x) {
    this->p_sh_addralign = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_entsize() const {
    return p_sh_entsize;
}

void
SgAsmElfSectionTableEntry::set_sh_entsize(rose_addr_t const& x) {
    this->p_sh_entsize = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfSectionTableEntry::get_extra() const {
    return p_extra;
}

SgUnsignedCharList&
SgAsmElfSectionTableEntry::get_extra() {
    return p_extra;
}

void
SgAsmElfSectionTableEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

SgAsmElfSectionTableEntry::~SgAsmElfSectionTableEntry() {
    destructorHelper();
}

SgAsmElfSectionTableEntry::SgAsmElfSectionTableEntry()
    : p_sh_name(0)
    , p_sh_type(SHT_PROGBITS)
    , p_sh_link(0)
    , p_sh_info(0)
    , p_sh_flags(0)
    , p_sh_addr(0)
    , p_sh_offset(0)
    , p_sh_size(0)
    , p_sh_addralign(0)
    , p_sh_entsize(0) {}

void
SgAsmElfSectionTableEntry::initializeProperties() {
    p_sh_name = 0;
    p_sh_type = SHT_PROGBITS;
    p_sh_link = 0;
    p_sh_info = 0;
    p_sh_flags = 0;
    p_sh_addr = 0;
    p_sh_offset = 0;
    p_sh_size = 0;
    p_sh_addralign = 0;
    p_sh_entsize = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
