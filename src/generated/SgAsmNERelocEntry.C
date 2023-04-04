#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNERelocEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::NERelocSrcType const&
SgAsmNERelocEntry::get_src_type() const {
    return p_src_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_src_type(SgAsmNERelocEntry::NERelocSrcType const& x) {
    this->p_src_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::NERelocModifiers const&
SgAsmNERelocEntry::get_modifier() const {
    return p_modifier;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_modifier(SgAsmNERelocEntry::NERelocModifiers const& x) {
    this->p_modifier = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::NERelocTgtType const&
SgAsmNERelocEntry::get_tgt_type() const {
    return p_tgt_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_tgt_type(SgAsmNERelocEntry::NERelocTgtType const& x) {
    this->p_tgt_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::NERelocFlags const&
SgAsmNERelocEntry::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_flags(SgAsmNERelocEntry::NERelocFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmNERelocEntry::get_src_offset() const {
    return p_src_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_src_offset(rose_addr_t const& x) {
    this->p_src_offset = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::iref_type const&
SgAsmNERelocEntry::get_iref() const {
    return p_iref;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_iref(SgAsmNERelocEntry::iref_type const& x) {
    this->p_iref = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::iord_type const&
SgAsmNERelocEntry::get_iord() const {
    return p_iord;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_iord(SgAsmNERelocEntry::iord_type const& x) {
    this->p_iord = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::iname_type const&
SgAsmNERelocEntry::get_iname() const {
    return p_iname;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_iname(SgAsmNERelocEntry::iname_type const& x) {
    this->p_iname = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::osfixup_type const&
SgAsmNERelocEntry::get_osfixup() const {
    return p_osfixup;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::set_osfixup(SgAsmNERelocEntry::osfixup_type const& x) {
    this->p_osfixup = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::~SgAsmNERelocEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocEntry::SgAsmNERelocEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_src_type(SgAsmNERelocEntry::RF_SRCTYPE_8OFF)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_modifier(SgAsmNERelocEntry::RF_MODIFIER_SINGLE)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_tgt_type(SgAsmNERelocEntry::RF_TGTTYPE_IREF)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(SgAsmNERelocEntry::RF_ADDITIVE)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_src_offset(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNERelocEntry::initializeProperties() {
    p_src_type = SgAsmNERelocEntry::RF_SRCTYPE_8OFF;
    p_modifier = SgAsmNERelocEntry::RF_MODIFIER_SINGLE;
    p_tgt_type = SgAsmNERelocEntry::RF_TGTTYPE_IREF;
    p_flags = SgAsmNERelocEntry::RF_ADDITIVE;
    p_src_offset = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
