//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilNestedClass            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilNestedClass_IMPL
#include <SgAsmCilNestedClass.h>

uint32_t const&
SgAsmCilNestedClass::get_NestedClass() const {
    return p_NestedClass;
}

void
SgAsmCilNestedClass::set_NestedClass(uint32_t const& x) {
    this->p_NestedClass = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilNestedClass::get_EnclosingClass() const {
    return p_EnclosingClass;
}

void
SgAsmCilNestedClass::set_EnclosingClass(uint32_t const& x) {
    this->p_EnclosingClass = x;
    set_isModified(true);
}

SgAsmCilNestedClass::~SgAsmCilNestedClass() {
    destructorHelper();
}

SgAsmCilNestedClass::SgAsmCilNestedClass()
    : p_NestedClass(0)
    , p_EnclosingClass(0) {}

void
SgAsmCilNestedClass::initializeProperties() {
    p_NestedClass = 0;
    p_EnclosingClass = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
