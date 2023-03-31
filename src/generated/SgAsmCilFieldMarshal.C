//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilFieldMarshal                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmCilFieldMarshal::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilFieldMarshal::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilFieldMarshal::get_NativeType() const {
    return p_NativeType;
}

void
SgAsmCilFieldMarshal::set_NativeType(uint32_t const& x) {
    this->p_NativeType = x;
    set_isModified(true);
}

SgAsmCilFieldMarshal::~SgAsmCilFieldMarshal() {
    destructorHelper();
}

SgAsmCilFieldMarshal::SgAsmCilFieldMarshal() {}

void
SgAsmCilFieldMarshal::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
