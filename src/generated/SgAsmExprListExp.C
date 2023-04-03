#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExprListExp            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList const&
SgAsmExprListExp::get_expressions() const {
    return p_expressions;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList&
SgAsmExprListExp::get_expressions() {
    return p_expressions;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExprListExp::set_expressions(SgAsmExpressionPtrList const& x) {
    this->p_expressions = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp::~SgAsmExprListExp() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp::SgAsmExprListExp() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExprListExp::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
