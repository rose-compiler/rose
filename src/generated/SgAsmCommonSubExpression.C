#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCommonSubExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmCommonSubExpression::get_subexpression() const {
    return p_subexpression;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCommonSubExpression::set_subexpression(SgAsmExpression* const& x) {
    this->p_subexpression = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCommonSubExpression::~SgAsmCommonSubExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCommonSubExpression::SgAsmCommonSubExpression()
    : p_subexpression(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCommonSubExpression::initializeProperties() {
    p_subexpression = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
