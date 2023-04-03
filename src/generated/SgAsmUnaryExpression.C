#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmUnaryExpression::get_operand() const {
    return p_operand;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmUnaryExpression::set_operand(SgAsmExpression* const& x) {
    this->p_operand = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::~SgAsmUnaryExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::SgAsmUnaryExpression()
    : p_operand(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmUnaryExpression::SgAsmUnaryExpression(SgAsmExpression* const& operand)
    : p_operand(operand) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmUnaryExpression::initializeProperties() {
    p_operand = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
