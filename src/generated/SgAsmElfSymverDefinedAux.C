#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedAux            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmElfSymverDefinedAux::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedAux::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedAux::~SgAsmElfSymverDefinedAux() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedAux::SgAsmElfSymverDefinedAux()
    : p_name(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedAux::initializeProperties() {
    p_name = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
