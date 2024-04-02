//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNEEntryPoint_IMPL
#include <SgAsmNEEntryPoint.h>

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
SgAsmNEEntryPoint::get_sectionIndex() const {
    return p_sectionIndex;
}

void
SgAsmNEEntryPoint::set_sectionIndex(unsigned const& x) {
    this->p_sectionIndex = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEEntryPoint::get_sectionOffset() const {
    return p_sectionOffset;
}

void
SgAsmNEEntryPoint::set_sectionOffset(unsigned const& x) {
    this->p_sectionOffset = x;
    set_isModified(true);
}

SgAsmNEEntryPoint::~SgAsmNEEntryPoint() {
    destructorHelper();
}

SgAsmNEEntryPoint::SgAsmNEEntryPoint()
    : p_flags(SgAsmNEEntryPoint::EF_ZERO)
    , p_int3f(0)
    , p_sectionIndex(0)
    , p_sectionOffset(0) {}

void
SgAsmNEEntryPoint::initializeProperties() {
    p_flags = SgAsmNEEntryPoint::EF_ZERO;
    p_int3f = 0;
    p_sectionIndex = 0;
    p_sectionOffset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
