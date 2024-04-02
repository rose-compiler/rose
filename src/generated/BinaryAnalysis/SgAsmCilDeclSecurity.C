//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilDeclSecurity            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilDeclSecurity_IMPL
#include <SgAsmCilDeclSecurity.h>

uint16_t const&
SgAsmCilDeclSecurity::get_Action() const {
    return p_Action;
}

void
SgAsmCilDeclSecurity::set_Action(uint16_t const& x) {
    this->p_Action = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilDeclSecurity::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilDeclSecurity::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilDeclSecurity::get_PermissionSet() const {
    return p_PermissionSet;
}

void
SgAsmCilDeclSecurity::set_PermissionSet(uint32_t const& x) {
    this->p_PermissionSet = x;
    set_isModified(true);
}

SgAsmCilDeclSecurity::~SgAsmCilDeclSecurity() {
    destructorHelper();
}

SgAsmCilDeclSecurity::SgAsmCilDeclSecurity()
    : p_Action(0)
    , p_Parent(0)
    , p_PermissionSet(0) {}

void
SgAsmCilDeclSecurity::initializeProperties() {
    p_Action = 0;
    p_Parent = 0;
    p_PermissionSet = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
