//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodImpl            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

SgAsmCilMethodImpl::SgAsmCilMethodImpl() {}

void
SgAsmCilMethodImpl::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
