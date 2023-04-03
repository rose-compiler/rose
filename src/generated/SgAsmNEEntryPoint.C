#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::NEEntryFlags const&
SgAsmNEEntryPoint::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_flags(SgAsmNEEntryPoint::NEEntryFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_int3f() const {
    return p_int3f;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_int3f(unsigned const& x) {
    this->p_int3f = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_section_idx() const {
    return p_section_idx;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_section_idx(unsigned const& x) {
    this->p_section_idx = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_section_offset() const {
    return p_section_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_section_offset(unsigned const& x) {
    this->p_section_offset = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::~SgAsmNEEntryPoint() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::SgAsmNEEntryPoint()
    : p_flags(SgAsmNEEntryPoint::EF_ZERO)
    , p_int3f(0)
    , p_section_idx(0)
    , p_section_offset(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::initializeProperties() {
    p_flags = SgAsmNEEntryPoint::EF_ZERO;
    p_int3f = 0;
    p_section_idx = 0;
    p_section_offset = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
