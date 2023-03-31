//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePair                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

rose_rva_t const&
SgAsmPERVASizePair::get_e_rva() const {
    return p_e_rva;
}

rose_rva_t&
SgAsmPERVASizePair::get_e_rva() {
    return p_e_rva;
}

void
SgAsmPERVASizePair::set_e_rva(rose_rva_t const& x) {
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
    : p_e_rva(0)
    , p_e_size(0)
    , p_section(nullptr) {}

void
SgAsmPERVASizePair::initializeProperties() {
    p_e_rva = 0;
    p_e_size = 0;
    p_section = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
