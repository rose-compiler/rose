#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmUnaryExpression::get_operand() const {
    return p_operand;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmUnaryExpression::set_operand(SgAsmExpression* const& x) {
    this->p_operand = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::~SgAsmUnaryExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::SgAsmUnaryExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_operand(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::SgAsmUnaryExpression(SgAsmExpression* const& operand)
    : p_operand(operand) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmUnaryExpression::initializeProperties() {
    p_operand = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
