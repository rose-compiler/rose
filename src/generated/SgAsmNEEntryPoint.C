//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryPoint                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmNEEntryPoint::NEEntryFlags const&
SgAsmNEEntryPoint::get_flags() const {
    return p_flags;
}

void
SgAsmNEEntryPoint::set_flags(SgAsmNEEntryPoint::NEEntryFlags const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEEntryPoint::get_int3f() const {
    return p_int3f;
}

void
SgAsmNEEntryPoint::set_int3f(unsigned const& x) {
    this->p_int3f = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEEntryPoint::get_section_idx() const {
    return p_section_idx;
}

void
SgAsmNEEntryPoint::set_section_idx(unsigned const& x) {
    this->p_section_idx = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEEntryPoint::get_section_offset() const {
    return p_section_offset;
}

void
SgAsmNEEntryPoint::set_section_offset(unsigned const& x) {
    this->p_section_offset = x;
    set_isModified(true);
}

SgAsmNEEntryPoint::~SgAsmNEEntryPoint() {
    destructorHelper();
}

SgAsmNEEntryPoint::SgAsmNEEntryPoint()
    : p_flags(SgAsmNEEntryPoint::EF_ZERO)
    , p_int3f(0)
    , p_section_idx(0)
    , p_section_offset(0) {}

void
SgAsmNEEntryPoint::initializeProperties() {
    p_flags = SgAsmNEEntryPoint::EF_ZERO;
    p_int3f = 0;
    p_section_idx = 0;
    p_section_offset = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
