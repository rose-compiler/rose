//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilNestedClass                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

SgAsmCilNestedClass::SgAsmCilNestedClass() {}

void
SgAsmCilNestedClass::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
