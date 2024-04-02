//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmRegisterReferenceExpression_IMPL
#include <sage3basic.h>

Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmRegisterReferenceExpression::get_descriptor() const {
    return p_descriptor;
}

void
SgAsmRegisterReferenceExpression::set_descriptor(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_descriptor = x;
    set_isModified(true);
}

int const&
SgAsmRegisterReferenceExpression::get_adjustment() const {
    return p_adjustment;
}

void
SgAsmRegisterReferenceExpression::set_adjustment(int const& x) {
    this->p_adjustment = x;
    set_isModified(true);
}

SgAsmRegisterReferenceExpression::~SgAsmRegisterReferenceExpression() {
    destructorHelper();
}

SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression()
    : p_adjustment(0) {}

// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor)
    : p_descriptor(descriptor)
    , p_adjustment(0) {}

void
SgAsmRegisterReferenceExpression::initializeProperties() {
    p_adjustment = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
