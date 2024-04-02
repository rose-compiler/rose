//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilFieldMarshal            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilFieldMarshal_IMPL
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

SgAsmCilFieldMarshal::SgAsmCilFieldMarshal()
    : p_Parent(0)
    , p_NativeType(0) {}

void
SgAsmCilFieldMarshal::initializeProperties() {
    p_Parent = 0;
    p_NativeType = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
