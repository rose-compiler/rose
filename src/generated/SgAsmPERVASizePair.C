#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePair            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPERVASizePair::get_e_rva() const {
    return p_e_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPERVASizePair::get_e_rva() {
    return p_e_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::set_e_rva(rose_rva_t const& x) {
    this->p_e_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmPERVASizePair::get_e_size() const {
    return p_e_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::set_e_size(rose_addr_t const& x) {
    this->p_e_size = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPERVASizePair::~SgAsmPERVASizePair() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPERVASizePair::SgAsmPERVASizePair()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_e_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_e_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_section(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPERVASizePair::initializeProperties() {
    p_e_rva = 0;
    p_e_size = 0;
    p_section = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
