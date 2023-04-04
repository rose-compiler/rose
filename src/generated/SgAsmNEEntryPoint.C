#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::NEEntryFlags const&
SgAsmNEEntryPoint::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_flags(SgAsmNEEntryPoint::NEEntryFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_int3f() const {
    return p_int3f;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_int3f(unsigned const& x) {
    this->p_int3f = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_section_idx() const {
    return p_section_idx;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_section_idx(unsigned const& x) {
    this->p_section_idx = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNEEntryPoint::get_section_offset() const {
    return p_section_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::set_section_offset(unsigned const& x) {
    this->p_section_offset = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::~SgAsmNEEntryPoint() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPoint::SgAsmNEEntryPoint()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_flags(SgAsmNEEntryPoint::EF_ZERO)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_int3f(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_section_idx(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_section_offset(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryPoint::initializeProperties() {
    p_flags = SgAsmNEEntryPoint::EF_ZERO;
    p_int3f = 0;
    p_section_idx = 0;
    p_section_offset = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
