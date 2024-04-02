//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilInterfaceImpl            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilInterfaceImpl_IMPL
#include <SgAsmCilInterfaceImpl.h>

uint32_t const&
SgAsmCilInterfaceImpl::get_Class() const {
    return p_Class;
}

void
SgAsmCilInterfaceImpl::set_Class(uint32_t const& x) {
    this->p_Class = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilInterfaceImpl::get_Interface() const {
    return p_Interface;
}

void
SgAsmCilInterfaceImpl::set_Interface(uint32_t const& x) {
    this->p_Interface = x;
    set_isModified(true);
}

SgAsmCilInterfaceImpl::~SgAsmCilInterfaceImpl() {
    destructorHelper();
}

SgAsmCilInterfaceImpl::SgAsmCilInterfaceImpl()
    : p_Class(0)
    , p_Interface(0) {}

void
SgAsmCilInterfaceImpl::initializeProperties() {
    p_Class = 0;
    p_Interface = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
