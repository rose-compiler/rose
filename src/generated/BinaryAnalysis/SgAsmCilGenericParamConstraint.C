//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilGenericParamConstraint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilGenericParamConstraint_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilGenericParamConstraint::get_Owner() const {
    return p_Owner;
}

void
SgAsmCilGenericParamConstraint::set_Owner(uint32_t const& x) {
    this->p_Owner = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilGenericParamConstraint::get_Constraint() const {
    return p_Constraint;
}

void
SgAsmCilGenericParamConstraint::set_Constraint(uint32_t const& x) {
    this->p_Constraint = x;
    set_isModified(true);
}

SgAsmCilGenericParamConstraint::~SgAsmCilGenericParamConstraint() {
    destructorHelper();
}

SgAsmCilGenericParamConstraint::SgAsmCilGenericParamConstraint()
    : p_Owner(0)
    , p_Constraint(0) {}

void
SgAsmCilGenericParamConstraint::initializeProperties() {
    p_Owner = 0;
    p_Constraint = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
