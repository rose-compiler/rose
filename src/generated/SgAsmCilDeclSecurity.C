//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilDeclSecurity                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

SgAsmCilDeclSecurity::SgAsmCilDeclSecurity() {}

void
SgAsmCilDeclSecurity::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
