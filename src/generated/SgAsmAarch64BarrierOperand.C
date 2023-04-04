#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64BarrierOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64BarrierOperation const&
SgAsmAarch64BarrierOperand::operation() const {
    return operation_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64BarrierOperand::operation(Rose::BinaryAnalysis::Aarch64BarrierOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64BarrierOperand::~SgAsmAarch64BarrierOperand() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64BarrierOperand::SgAsmAarch64BarrierOperand() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64BarrierOperand
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64BarrierOperand::SgAsmAarch64BarrierOperand(Rose::BinaryAnalysis::Aarch64BarrierOperation const& operation)
    : operation_(operation) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64BarrierOperand::initializeProperties() {
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
