#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmBinaryExpression::get_lhs() const {
    return p_lhs;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::set_lhs(SgAsmExpression* const& x) {
    this->p_lhs = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmBinaryExpression::get_rhs() const {
    return p_rhs;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::set_rhs(SgAsmExpression* const& x) {
    this->p_rhs = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::~SgAsmBinaryExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::SgAsmBinaryExpression()
    : p_lhs(nullptr)
    , p_rhs(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::SgAsmBinaryExpression(SgAsmExpression* const& lhs,
                                             SgAsmExpression* const& rhs)
    : p_lhs(lhs)
    , p_rhs(rhs) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::initializeProperties() {
    p_lhs = nullptr;
    p_rhs = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
