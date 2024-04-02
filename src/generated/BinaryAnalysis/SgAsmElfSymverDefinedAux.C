//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedAux            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverDefinedAux_IMPL
#include <sage3basic.h>

SgAsmGenericString* const&
SgAsmElfSymverDefinedAux::get_name() const {
    return p_name;
}

void
SgAsmElfSymverDefinedAux::set_name(SgAsmGenericString* const& x) {
    changeChildPointer(this->p_name, const_cast<SgAsmGenericString*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
