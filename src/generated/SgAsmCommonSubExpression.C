#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCommonSubExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmCommonSubExpression::get_subexpression() const {
    return p_subexpression;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCommonSubExpression::set_subexpression(SgAsmExpression* const& x) {
    this->p_subexpression = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCommonSubExpression::~SgAsmCommonSubExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCommonSubExpression::SgAsmCommonSubExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_subexpression(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCommonSubExpression::initializeProperties() {
    p_subexpression = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
