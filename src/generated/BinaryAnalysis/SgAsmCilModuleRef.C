//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilModuleRef            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilModuleRef_IMPL
#include <SgAsmCilModuleRef.h>

uint32_t const&
SgAsmCilModuleRef::get_Name() const {
    return p_Name;
}

void
SgAsmCilModuleRef::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

SgAsmCilModuleRef::~SgAsmCilModuleRef() {
    destructorHelper();
}

SgAsmCilModuleRef::SgAsmCilModuleRef()
    : p_Name(0) {}

void
SgAsmCilModuleRef::initializeProperties() {
    p_Name = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
