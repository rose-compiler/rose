//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNERelocEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNERelocEntry_IMPL
#include <SgAsmNERelocEntry.h>

SgAsmNERelocEntry::NERelocSrcType const&
SgAsmNERelocEntry::get_src_type() const {
    return p_src_type;
}

void
SgAsmNERelocEntry::set_src_type(SgAsmNERelocEntry::NERelocSrcType const& x) {
    this->p_src_type = x;
    set_isModified(true);
}

SgAsmNERelocEntry::NERelocModifiers const&
SgAsmNERelocEntry::get_modifier() const {
    return p_modifier;
}

void
SgAsmNERelocEntry::set_modifier(SgAsmNERelocEntry::NERelocModifiers const& x) {
    this->p_modifier = x;
    set_isModified(true);
}

SgAsmNERelocEntry::NERelocTgtType const&
SgAsmNERelocEntry::get_tgt_type() const {
    return p_tgt_type;
}

void
SgAsmNERelocEntry::set_tgt_type(SgAsmNERelocEntry::NERelocTgtType const& x) {
    this->p_tgt_type = x;
    set_isModified(true);
}

SgAsmNERelocEntry::NERelocFlags const&
SgAsmNERelocEntry::get_flags() const {
    return p_flags;
}

void
SgAsmNERelocEntry::set_flags(SgAsmNERelocEntry::NERelocFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmNERelocEntry::get_src_offset() const {
    return p_src_offset;
}

void
SgAsmNERelocEntry::set_src_offset(Rose::BinaryAnalysis::Address const& x) {
    this->p_src_offset = x;
    set_isModified(true);
}

SgAsmNERelocEntry::iref_type const&
SgAsmNERelocEntry::get_iref() const {
    return p_iref;
}

void
SgAsmNERelocEntry::set_iref(SgAsmNERelocEntry::iref_type const& x) {
    this->p_iref = x;
    set_isModified(true);
}

SgAsmNERelocEntry::iord_type const&
SgAsmNERelocEntry::get_iord() const {
    return p_iord;
}

void
SgAsmNERelocEntry::set_iord(SgAsmNERelocEntry::iord_type const& x) {
    this->p_iord = x;
    set_isModified(true);
}

SgAsmNERelocEntry::iname_type const&
SgAsmNERelocEntry::get_iname() const {
    return p_iname;
}

void
SgAsmNERelocEntry::set_iname(SgAsmNERelocEntry::iname_type const& x) {
    this->p_iname = x;
    set_isModified(true);
}

SgAsmNERelocEntry::osfixup_type const&
SgAsmNERelocEntry::get_osfixup() const {
    return p_osfixup;
}

void
SgAsmNERelocEntry::set_osfixup(SgAsmNERelocEntry::osfixup_type const& x) {
    this->p_osfixup = x;
    set_isModified(true);
}

SgAsmNERelocEntry::~SgAsmNERelocEntry() {
    destructorHelper();
}

SgAsmNERelocEntry::SgAsmNERelocEntry()
    : p_src_type(SgAsmNERelocEntry::RF_SRCTYPE_8OFF)
    , p_modifier(SgAsmNERelocEntry::RF_MODIFIER_SINGLE)
    , p_tgt_type(SgAsmNERelocEntry::RF_TGTTYPE_IREF)
    , p_flags(SgAsmNERelocEntry::RF_ADDITIVE)
    , p_src_offset(0) {}

void
SgAsmNERelocEntry::initializeProperties() {
    p_src_type = SgAsmNERelocEntry::RF_SRCTYPE_8OFF;
    p_modifier = SgAsmNERelocEntry::RF_MODIFIER_SINGLE;
    p_tgt_type = SgAsmNERelocEntry::RF_TGTTYPE_IREF;
    p_flags = SgAsmNERelocEntry::RF_ADDITIVE;
    p_src_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
