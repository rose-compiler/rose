#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSegmentTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSegmentTableEntry::get_index() const {
    return p_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry::SegmentType const&
SgAsmElfSegmentTableEntry::get_type() const {
    return p_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_type(SgAsmElfSegmentTableEntry::SegmentType const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry::SegmentFlags const&
SgAsmElfSegmentTableEntry::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_flags(SgAsmElfSegmentTableEntry::SegmentFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_offset() const {
    return p_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_offset(rose_addr_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_vaddr() const {
    return p_vaddr;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_vaddr(rose_addr_t const& x) {
    this->p_vaddr = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_paddr() const {
    return p_paddr;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_paddr(rose_addr_t const& x) {
    this->p_paddr = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_filesz() const {
    return p_filesz;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_filesz(rose_addr_t const& x) {
    this->p_filesz = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_memsz() const {
    return p_memsz;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_memsz(rose_addr_t const& x) {
    this->p_memsz = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSegmentTableEntry::get_align() const {
    return p_align;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_align(rose_addr_t const& x) {
    this->p_align = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfSegmentTableEntry::get_extra() const {
    return p_extra;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfSegmentTableEntry::get_extra() {
    return p_extra;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry::~SgAsmElfSegmentTableEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry::SgAsmElfSegmentTableEntry()
    : p_index(0)
    , p_type(SgAsmElfSegmentTableEntry::PT_LOAD)
    , p_flags(SgAsmElfSegmentTableEntry::PF_NONE)
    , p_offset(0)
    , p_vaddr(0)
    , p_paddr(0)
    , p_filesz(0)
    , p_memsz(0)
    , p_align(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSegmentTableEntry::initializeProperties() {
    p_index = 0;
    p_type = SgAsmElfSegmentTableEntry::PT_LOAD;
    p_flags = SgAsmElfSegmentTableEntry::PF_NONE;
    p_offset = 0;
    p_vaddr = 0;
    p_paddr = 0;
    p_filesz = 0;
    p_memsz = 0;
    p_align = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
