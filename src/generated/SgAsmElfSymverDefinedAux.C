//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedAux                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericString* const&
SgAsmElfSymverDefinedAux::get_name() const {
    return p_name;
}

void
SgAsmElfSymverDefinedAux::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

SgAsmElfSymverDefinedAux::~SgAsmElfSymverDefinedAux() {
    destructorHelper();
}

SgAsmElfSymverDefinedAux::SgAsmElfSymverDefinedAux()
    : p_name(0) {}

void
SgAsmElfSymverDefinedAux::initializeProperties() {
    p_name = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
