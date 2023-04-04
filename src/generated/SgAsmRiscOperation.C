#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRiscOperation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::RiscOperator const&
SgAsmRiscOperation::get_riscOperator() const {
    return p_riscOperator;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::set_riscOperator(SgAsmRiscOperation::RiscOperator const& x) {
    this->p_riscOperator = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExprListExp* const&
SgAsmRiscOperation::get_operands() const {
    return p_operands;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::set_operands(SgAsmExprListExp* const& x) {
    this->p_operands = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::~SgAsmRiscOperation() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::SgAsmRiscOperation()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_riscOperator(OP_NONE)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_operands(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=riscOperator     class=SgAsmRiscOperation
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmRiscOperation::SgAsmRiscOperation(SgAsmRiscOperation::RiscOperator const& riscOperator)
    : p_riscOperator(riscOperator)
    , p_operands(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRiscOperation::initializeProperties() {
    p_riscOperator = OP_NONE;
    p_operands = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
