#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmPESectionTableEntry::get_name() const {
    return p_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPESectionTableEntry::get_virtual_size() const {
    return p_virtual_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_virtual_size(rose_addr_t const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPESectionTableEntry::get_rva() const {
    return p_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_rva(rose_addr_t const& x) {
    this->p_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPESectionTableEntry::get_physical_size() const {
    return p_physical_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_physical_size(rose_addr_t const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPESectionTableEntry::get_physical_offset() const {
    return p_physical_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_physical_offset(rose_addr_t const& x) {
    this->p_physical_offset = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPESectionTableEntry::get_coff_line_nums() const {
    return p_coff_line_nums;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_coff_line_nums(unsigned const& x) {
    this->p_coff_line_nums = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPESectionTableEntry::get_n_relocs() const {
    return p_n_relocs;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_n_relocs(unsigned const& x) {
    this->p_n_relocs = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPESectionTableEntry::get_n_coff_line_nums() const {
    return p_n_coff_line_nums;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_n_coff_line_nums(unsigned const& x) {
    this->p_n_coff_line_nums = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPESectionTableEntry::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPESectionTableEntry::~SgAsmPESectionTableEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPESectionTableEntry::SgAsmPESectionTableEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_virtual_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_physical_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_physical_offset(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_coff_line_nums(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_n_relocs(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_n_coff_line_nums(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
