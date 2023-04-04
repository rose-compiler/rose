#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfRelocEntry::get_r_offset() const {
    return p_r_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::set_r_offset(rose_addr_t const& x) {
    this->p_r_offset = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfRelocEntry::get_r_addend() const {
    return p_r_addend;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::set_r_addend(rose_addr_t const& x) {
    this->p_r_addend = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfRelocEntry::get_sym() const {
    return p_sym;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::set_sym(unsigned long const& x) {
    this->p_sym = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocEntry::RelocType const&
SgAsmElfRelocEntry::get_type() const {
    return p_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::set_type(SgAsmElfRelocEntry::RelocType const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfRelocEntry::get_extra() const {
    return p_extra;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfRelocEntry::get_extra() {
    return p_extra;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocEntry::~SgAsmElfRelocEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocEntry::SgAsmElfRelocEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_r_offset(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_r_addend(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sym(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_type(R_386_NONE) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocEntry::initializeProperties() {
    p_r_offset = 0;
    p_r_addend = 0;
    p_sym = 0;
    p_type = R_386_NONE;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
