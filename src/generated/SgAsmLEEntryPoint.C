#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList const&
SgAsmLEEntryPoint::get_entries() const {
    return p_entries;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList&
SgAsmLEEntryPoint::get_entries() {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entries(SgAsmLEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_objnum() const {
    return p_objnum;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_objnum(unsigned const& x) {
    this->p_objnum = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_entry_type() const {
    return p_entry_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entry_type(unsigned const& x) {
    this->p_entry_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEEntryPoint::get_res1() const {
    return p_res1;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLEEntryPoint::get_entry_offset() const {
    return p_entry_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::set_entry_offset(rose_addr_t const& x) {
    this->p_entry_offset = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPoint::~SgAsmLEEntryPoint() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPoint::SgAsmLEEntryPoint()
    : p_flags(0)
    , p_objnum(0)
    , p_entry_type(0)
    , p_res1(0)
    , p_entry_offset(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryPoint::initializeProperties() {
    p_flags = 0;
    p_objnum = 0;
    p_entry_type = 0;
    p_res1 = 0;
    p_entry_offset = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
