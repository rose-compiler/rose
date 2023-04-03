#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmRegisterReferenceExpression::get_descriptor() const {
    return p_descriptor;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::set_descriptor(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_descriptor = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmRegisterReferenceExpression::get_adjustment() const {
    return p_adjustment;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::set_adjustment(int const& x) {
    this->p_adjustment = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::~SgAsmRegisterReferenceExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression()
    : p_adjustment(0) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpression::SgAsmRegisterReferenceExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor)
    : p_descriptor(descriptor) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterReferenceExpression::initializeProperties() {
    p_adjustment = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
