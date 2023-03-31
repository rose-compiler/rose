//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededAux                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmElfSymverNeededAux::get_hash() const {
    return p_hash;
}

void
SgAsmElfSymverNeededAux::set_hash(uint32_t const& x) {
    this->p_hash = x;
    set_isModified(true);
}

int const&
SgAsmElfSymverNeededAux::get_flags() const {
    return p_flags;
}

void
SgAsmElfSymverNeededAux::set_flags(int const& x) {
    this->p_flags = x;
    set_isModified(true);
}

size_t const&
SgAsmElfSymverNeededAux::get_other() const {
    return p_other;
}

void
SgAsmElfSymverNeededAux::set_other(size_t const& x) {
    this->p_other = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmElfSymverNeededAux::get_name() const {
    return p_name;
}

void
SgAsmElfSymverNeededAux::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

SgAsmElfSymverNeededAux::~SgAsmElfSymverNeededAux() {
    destructorHelper();
}

SgAsmElfSymverNeededAux::SgAsmElfSymverNeededAux()
    : p_hash(0)
    , p_flags(0)
    , p_other(0)
    , p_name(0) {}

void
SgAsmElfSymverNeededAux::initializeProperties() {
    p_hash = 0;
    p_flags = 0;
    p_other = 0;
    p_name = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
