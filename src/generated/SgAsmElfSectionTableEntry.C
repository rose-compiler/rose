#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfSectionTableEntry::get_sh_name() const {
    return p_sh_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_name(unsigned const& x) {
    this->p_sh_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry::SectionType const&
SgAsmElfSectionTableEntry::get_sh_type() const {
    return p_sh_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_type(SgAsmElfSectionTableEntry::SectionType const& x) {
    this->p_sh_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfSectionTableEntry::get_sh_link() const {
    return p_sh_link;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_link(unsigned long const& x) {
    this->p_sh_link = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfSectionTableEntry::get_sh_info() const {
    return p_sh_info;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_info(unsigned long const& x) {
    this->p_sh_info = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmElfSectionTableEntry::get_sh_flags() const {
    return p_sh_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_flags(uint64_t const& x) {
    this->p_sh_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addr() const {
    return p_sh_addr;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_addr(rose_addr_t const& x) {
    this->p_sh_addr = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_offset() const {
    return p_sh_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_offset(rose_addr_t const& x) {
    this->p_sh_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_size() const {
    return p_sh_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_size(rose_addr_t const& x) {
    this->p_sh_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addralign() const {
    return p_sh_addralign;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_addralign(rose_addr_t const& x) {
    this->p_sh_addralign = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_entsize() const {
    return p_sh_entsize;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_entsize(rose_addr_t const& x) {
    this->p_sh_entsize = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfSectionTableEntry::get_extra() const {
    return p_extra;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfSectionTableEntry::get_extra() {
    return p_extra;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry::~SgAsmElfSectionTableEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
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

#line 389 "src/Rosebud/RosettaGenerator.C"
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

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
