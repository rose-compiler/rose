//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilTypeRef            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilTypeRef_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmCilTypeRef::get_ResolutionScope() const {
    return p_ResolutionScope;
}

void
SgAsmCilTypeRef::set_ResolutionScope(uint16_t const& x) {
    this->p_ResolutionScope = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeRef::get_TypeName() const {
    return p_TypeName;
}

void
SgAsmCilTypeRef::set_TypeName(uint32_t const& x) {
    this->p_TypeName = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeRef::get_TypeNamespace() const {
    return p_TypeNamespace;
}

void
SgAsmCilTypeRef::set_TypeNamespace(uint32_t const& x) {
    this->p_TypeNamespace = x;
    set_isModified(true);
}

SgAsmCilTypeRef::~SgAsmCilTypeRef() {
    destructorHelper();
}

SgAsmCilTypeRef::SgAsmCilTypeRef()
    : p_ResolutionScope(0)
    , p_TypeName(0)
    , p_TypeNamespace(0) {}

void
SgAsmCilTypeRef::initializeProperties() {
    p_ResolutionScope = 0;
    p_TypeName = 0;
    p_TypeNamespace = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
