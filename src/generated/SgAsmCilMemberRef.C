//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMemberRef                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmCilMemberRef::get_Class() const {
    return p_Class;
}

void
SgAsmCilMemberRef::set_Class(uint32_t const& x) {
    this->p_Class = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMemberRef::get_Name() const {
    return p_Name;
}

void
SgAsmCilMemberRef::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMemberRef::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilMemberRef::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

SgAsmCilMemberRef::~SgAsmCilMemberRef() {
    destructorHelper();
}

SgAsmCilMemberRef::SgAsmCilMemberRef() {}

void
SgAsmCilMemberRef::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
