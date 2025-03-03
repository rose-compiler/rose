//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSegmentTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSegmentTableEntry_IMPL
#include <SgAsmElfSegmentTableEntry.h>

size_t const&
SgAsmElfSegmentTableEntry::get_index() const {
    return p_index;
}

void
SgAsmElfSegmentTableEntry::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

SgAsmElfSegmentTableEntry::SegmentType const&
SgAsmElfSegmentTableEntry::get_type() const {
    return p_type;
}

void
SgAsmElfSegmentTableEntry::set_type(SgAsmElfSegmentTableEntry::SegmentType const& x) {
    this->p_type = x;
    set_isModified(true);
}

SgAsmElfSegmentTableEntry::SegmentFlags const&
SgAsmElfSegmentTableEntry::get_flags() const {
    return p_flags;
}

void
SgAsmElfSegmentTableEntry::set_flags(SgAsmElfSegmentTableEntry::SegmentFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_offset() const {
    return p_offset;
}

void
SgAsmElfSegmentTableEntry::set_offset(Rose::BinaryAnalysis::Address const& x) {
    this->p_offset = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_vaddr() const {
    return p_vaddr;
}

void
SgAsmElfSegmentTableEntry::set_vaddr(Rose::BinaryAnalysis::Address const& x) {
    this->p_vaddr = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_paddr() const {
    return p_paddr;
}

void
SgAsmElfSegmentTableEntry::set_paddr(Rose::BinaryAnalysis::Address const& x) {
    this->p_paddr = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_filesz() const {
    return p_filesz;
}

void
SgAsmElfSegmentTableEntry::set_filesz(Rose::BinaryAnalysis::Address const& x) {
    this->p_filesz = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_memsz() const {
    return p_memsz;
}

void
SgAsmElfSegmentTableEntry::set_memsz(Rose::BinaryAnalysis::Address const& x) {
    this->p_memsz = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmElfSegmentTableEntry::get_align() const {
    return p_align;
}

void
SgAsmElfSegmentTableEntry::set_align(Rose::BinaryAnalysis::Address const& x) {
    this->p_align = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfSegmentTableEntry::get_extra() const {
    return p_extra;
}

SgUnsignedCharList&
SgAsmElfSegmentTableEntry::get_extra() {
    return p_extra;
}

void
SgAsmElfSegmentTableEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

SgAsmElfSegmentTableEntry::~SgAsmElfSegmentTableEntry() {
    destructorHelper();
}

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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
