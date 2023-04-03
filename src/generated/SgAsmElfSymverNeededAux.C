#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededAux            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmElfSymverNeededAux::get_hash() const {
    return p_hash;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededAux::set_hash(uint32_t const& x) {
    this->p_hash = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfSymverNeededAux::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededAux::set_flags(int const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverNeededAux::get_other() const {
    return p_other;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededAux::set_other(size_t const& x) {
    this->p_other = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmElfSymverNeededAux::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededAux::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededAux::~SgAsmElfSymverNeededAux() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededAux::SgAsmElfSymverNeededAux()
    : p_hash(0)
    , p_flags(0)
    , p_other(0)
    , p_name(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededAux::initializeProperties() {
    p_hash = 0;
    p_flags = 0;
    p_other = 0;
    p_name = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
