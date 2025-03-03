//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPESectionTableEntry_IMPL
#include <SgAsmPESectionTableEntry.h>

std::string const&
SgAsmPESectionTableEntry::get_name() const {
    return p_name;
}

void
SgAsmPESectionTableEntry::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmPESectionTableEntry::get_virtual_size() const {
    return p_virtual_size;
}

void
SgAsmPESectionTableEntry::set_virtual_size(Rose::BinaryAnalysis::Address const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmPESectionTableEntry::get_rva() const {
    return p_rva;
}

void
SgAsmPESectionTableEntry::set_rva(Rose::BinaryAnalysis::Address const& x) {
    this->p_rva = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmPESectionTableEntry::get_physical_size() const {
    return p_physical_size;
}

void
SgAsmPESectionTableEntry::set_physical_size(Rose::BinaryAnalysis::Address const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmPESectionTableEntry::get_physical_offset() const {
    return p_physical_offset;
}

void
SgAsmPESectionTableEntry::set_physical_offset(Rose::BinaryAnalysis::Address const& x) {
    this->p_physical_offset = x;
    set_isModified(true);
}

unsigned const&
SgAsmPESectionTableEntry::get_coff_line_nums() const {
    return p_coff_line_nums;
}

void
SgAsmPESectionTableEntry::set_coff_line_nums(unsigned const& x) {
    this->p_coff_line_nums = x;
    set_isModified(true);
}

unsigned const&
SgAsmPESectionTableEntry::get_n_relocs() const {
    return p_n_relocs;
}

void
SgAsmPESectionTableEntry::set_n_relocs(unsigned const& x) {
    this->p_n_relocs = x;
    set_isModified(true);
}

unsigned const&
SgAsmPESectionTableEntry::get_n_coff_line_nums() const {
    return p_n_coff_line_nums;
}

void
SgAsmPESectionTableEntry::set_n_coff_line_nums(unsigned const& x) {
    this->p_n_coff_line_nums = x;
    set_isModified(true);
}

unsigned const&
SgAsmPESectionTableEntry::get_flags() const {
    return p_flags;
}

void
SgAsmPESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

SgAsmPESectionTableEntry::~SgAsmPESectionTableEntry() {
    destructorHelper();
}

SgAsmPESectionTableEntry::SgAsmPESectionTableEntry()
    : p_virtual_size(0)
    , p_rva(0)
    , p_physical_size(0)
    , p_physical_offset(0)
    , p_coff_line_nums(0)
    , p_n_relocs(0)
    , p_n_coff_line_nums(0)
    , p_flags(0) {}

void
SgAsmPESectionTableEntry::initializeProperties() {
    p_virtual_size = 0;
    p_rva = 0;
    p_physical_size = 0;
    p_physical_offset = 0;
    p_coff_line_nums = 0;
    p_n_relocs = 0;
    p_n_coff_line_nums = 0;
    p_flags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
