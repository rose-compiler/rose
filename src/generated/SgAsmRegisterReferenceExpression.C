#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmRegisterReferenceExpression::get_descriptor() const {
    return p_descriptor;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::set_descriptor(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_descriptor = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmRegisterReferenceExpression::get_adjustment() const {
    return p_adjustment;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::set_adjustment(int const& x) {
    this->p_adjustment = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::~SgAsmRegisterReferenceExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_adjustment(0) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor)
    : p_descriptor(descriptor)
    , p_adjustment(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::initializeProperties() {
    p_adjustment = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
