//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64AtOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64AtOperand_IMPL
#include <sage3basic.h>

Rose::BinaryAnalysis::Aarch64AtOperation const&
SgAsmAarch64AtOperand::operation() const {
    return operation_;
}

void
SgAsmAarch64AtOperand::operation(Rose::BinaryAnalysis::Aarch64AtOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

SgAsmAarch64AtOperand::~SgAsmAarch64AtOperand() {
    destructorHelper();
}

SgAsmAarch64AtOperand::SgAsmAarch64AtOperand() {}

// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64AtOperand
SgAsmAarch64AtOperand::SgAsmAarch64AtOperand(Rose::BinaryAnalysis::Aarch64AtOperation const& operation)
    : operation_(operation) {}

void
SgAsmAarch64AtOperand::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
