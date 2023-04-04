#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList const&
SgAsmLEEntryPoint::get_entries() const {
    return p_entries;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList&
SgAsmLEEntryPoint::get_entries() {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entries(SgAsmLEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_objnum() const {
    return p_objnum;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_objnum(unsigned const& x) {
    this->p_objnum = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_entry_type() const {
    return p_entry_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entry_type(unsigned const& x) {
    this->p_entry_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_res1() const {
    return p_res1;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLEEntryPoint::get_entry_offset() const {
    return p_entry_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entry_offset(rose_addr_t const& x) {
    this->p_entry_offset = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPoint::~SgAsmLEEntryPoint() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPoint::SgAsmLEEntryPoint()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_objnum(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entry_type(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_res1(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entry_offset(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::initializeProperties() {
    p_flags = 0;
    p_objnum = 0;
    p_entry_type = 0;
    p_res1 = 0;
    p_entry_offset = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
