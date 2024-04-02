//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64BarrierOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64BarrierOperand_IMPL
#include <SgAsmAarch64BarrierOperand.h>

Rose::BinaryAnalysis::Aarch64BarrierOperation const&
SgAsmAarch64BarrierOperand::operation() const {
    return operation_;
}

void
SgAsmAarch64BarrierOperand::operation(Rose::BinaryAnalysis::Aarch64BarrierOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

SgAsmAarch64BarrierOperand::~SgAsmAarch64BarrierOperand() {
    destructorHelper();
}

SgAsmAarch64BarrierOperand::SgAsmAarch64BarrierOperand() {}

// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64BarrierOperand
SgAsmAarch64BarrierOperand::SgAsmAarch64BarrierOperand(Rose::BinaryAnalysis::Aarch64BarrierOperation const& operation)
    : operation_(operation) {}

void
SgAsmAarch64BarrierOperand::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
