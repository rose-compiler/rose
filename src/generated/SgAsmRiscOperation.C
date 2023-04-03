#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRiscOperation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::RiscOperator const&
SgAsmRiscOperation::get_riscOperator() const {
    return p_riscOperator;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::set_riscOperator(SgAsmRiscOperation::RiscOperator const& x) {
    this->p_riscOperator = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp* const&
SgAsmRiscOperation::get_operands() const {
    return p_operands;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::set_operands(SgAsmExprListExp* const& x) {
    this->p_operands = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::~SgAsmRiscOperation() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::SgAsmRiscOperation()
    : p_riscOperator(OP_NONE)
    , p_operands(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=riscOperator     class=SgAsmRiscOperation
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::SgAsmRiscOperation(SgAsmRiscOperation::RiscOperator const& riscOperator)
    : p_riscOperator(riscOperator) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::initializeProperties() {
    p_riscOperator = OP_NONE;
    p_operands = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
