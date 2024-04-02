//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodImpl            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodImpl_IMPL
#include <SgAsmCilMethodImpl.h>

uint32_t const&
SgAsmCilMethodImpl::get_Class() const {
    return p_Class;
}

void
SgAsmCilMethodImpl::set_Class(uint32_t const& x) {
    this->p_Class = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodImpl::get_MethodBody() const {
    return p_MethodBody;
}

void
SgAsmCilMethodImpl::set_MethodBody(uint32_t const& x) {
    this->p_MethodBody = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodImpl::get_MethodDeclaration() const {
    return p_MethodDeclaration;
}

void
SgAsmCilMethodImpl::set_MethodDeclaration(uint32_t const& x) {
    this->p_MethodDeclaration = x;
    set_isModified(true);
}

SgAsmCilMethodImpl::~SgAsmCilMethodImpl() {
    destructorHelper();
}

SgAsmCilMethodImpl::SgAsmCilMethodImpl()
    : p_Class(0)
    , p_MethodBody(0)
    , p_MethodDeclaration(0) {}

void
SgAsmCilMethodImpl::initializeProperties() {
    p_Class = 0;
    p_MethodBody = 0;
    p_MethodDeclaration = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
