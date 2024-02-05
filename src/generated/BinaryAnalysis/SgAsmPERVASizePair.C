//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePair            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPERVASizePair::get_e_rva() const {
    return p_e_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPERVASizePair::get_e_rva() {
    return p_e_rva;
}

void
SgAsmPERVASizePair::set_e_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_e_rva = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmPERVASizePair::get_e_size() const {
    return p_e_size;
}

void
SgAsmPERVASizePair::set_e_size(rose_addr_t const& x) {
    this->p_e_size = x;
    set_isModified(true);
}

SgAsmPERVASizePair::~SgAsmPERVASizePair() {
    destructorHelper();
}

SgAsmPERVASizePair::SgAsmPERVASizePair()
    : p_e_size(0)
    , p_section(nullptr) {}

void
SgAsmPERVASizePair::initializeProperties() {
    p_e_size = 0;
    p_section = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
