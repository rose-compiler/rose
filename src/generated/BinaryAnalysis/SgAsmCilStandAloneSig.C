//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilStandAloneSig            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint32_t const&
SgAsmCilStandAloneSig::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilStandAloneSig::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

SgAsmCilStandAloneSig::~SgAsmCilStandAloneSig() {
    destructorHelper();
}

SgAsmCilStandAloneSig::SgAsmCilStandAloneSig()
    : p_Signature({}) {}

void
SgAsmCilStandAloneSig::initializeProperties() {
    p_Signature = {};
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
