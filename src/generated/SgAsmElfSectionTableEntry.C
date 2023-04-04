#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfSectionTableEntry::get_sh_name() const {
    return p_sh_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_name(unsigned const& x) {
    this->p_sh_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry::SectionType const&
SgAsmElfSectionTableEntry::get_sh_type() const {
    return p_sh_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_type(SgAsmElfSectionTableEntry::SectionType const& x) {
    this->p_sh_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfSectionTableEntry::get_sh_link() const {
    return p_sh_link;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_link(unsigned long const& x) {
    this->p_sh_link = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfSectionTableEntry::get_sh_info() const {
    return p_sh_info;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_info(unsigned long const& x) {
    this->p_sh_info = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmElfSectionTableEntry::get_sh_flags() const {
    return p_sh_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_flags(uint64_t const& x) {
    this->p_sh_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addr() const {
    return p_sh_addr;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_addr(rose_addr_t const& x) {
    this->p_sh_addr = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_offset() const {
    return p_sh_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_offset(rose_addr_t const& x) {
    this->p_sh_offset = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_size() const {
    return p_sh_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_size(rose_addr_t const& x) {
    this->p_sh_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_addralign() const {
    return p_sh_addralign;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_addralign(rose_addr_t const& x) {
    this->p_sh_addralign = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSectionTableEntry::get_sh_entsize() const {
    return p_sh_entsize;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_sh_entsize(rose_addr_t const& x) {
    this->p_sh_entsize = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfSectionTableEntry::get_extra() const {
    return p_extra;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfSectionTableEntry::get_extra() {
    return p_extra;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSectionTableEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry::~SgAsmElfSectionTableEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry::SgAsmElfSectionTableEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_sh_name(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_type(SHT_PROGBITS)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_link(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_info(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_addr(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_offset(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_addralign(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sh_entsize(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
