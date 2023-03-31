//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilGenericParamConstraint                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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

SgAsmCilGenericParamConstraint::SgAsmCilGenericParamConstraint() {}

void
SgAsmCilGenericParamConstraint::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
