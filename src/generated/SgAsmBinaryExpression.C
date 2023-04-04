#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmBinaryExpression::get_lhs() const {
    return p_lhs;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::set_lhs(SgAsmExpression* const& x) {
    this->p_lhs = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmBinaryExpression::get_rhs() const {
    return p_rhs;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::set_rhs(SgAsmExpression* const& x) {
    this->p_rhs = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::~SgAsmBinaryExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::SgAsmBinaryExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_lhs(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_rhs(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmBinaryExpression::SgAsmBinaryExpression(SgAsmExpression* const& lhs,
                                             SgAsmExpression* const& rhs)
    : p_lhs(lhs)
    , p_rhs(rhs) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmBinaryExpression::initializeProperties() {
    p_lhs = nullptr;
    p_rhs = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
