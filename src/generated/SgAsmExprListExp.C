#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExprListExp            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList const&
SgAsmExprListExp::get_expressions() const {
    return p_expressions;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList&
SgAsmExprListExp::get_expressions() {
    return p_expressions;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExprListExp::set_expressions(SgAsmExpressionPtrList const& x) {
    this->p_expressions = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp::~SgAsmExprListExp() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp::SgAsmExprListExp() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExprListExp::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
