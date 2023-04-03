#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePair            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPERVASizePair::get_e_rva() const {
    return p_e_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPERVASizePair::get_e_rva() {
    return p_e_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::set_e_rva(rose_rva_t const& x) {
    this->p_e_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPERVASizePair::get_e_size() const {
    return p_e_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::set_e_size(rose_addr_t const& x) {
    this->p_e_size = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPERVASizePair::~SgAsmPERVASizePair() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPERVASizePair::SgAsmPERVASizePair()
    : p_e_rva(0)
    , p_e_size(0)
    , p_section(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::initializeProperties() {
    p_e_rva = 0;
    p_e_size = 0;
    p_section = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
