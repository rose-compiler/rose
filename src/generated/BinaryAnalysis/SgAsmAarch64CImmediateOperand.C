//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64CImmediateOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64CImmediateOperand_IMPL
#include <SgAsmAarch64CImmediateOperand.h>

unsigned const&
SgAsmAarch64CImmediateOperand::immediate() const {
    return immediate_;
}

void
SgAsmAarch64CImmediateOperand::immediate(unsigned const& x) {
    this->immediate_ = x;
    set_isModified(true);
}

SgAsmAarch64CImmediateOperand::~SgAsmAarch64CImmediateOperand() {
    destructorHelper();
}

SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand() {}

// The association between constructor arguments and their classes:
//    property=immediate        class=SgAsmAarch64CImmediateOperand
SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand(unsigned const& immediate)
    : immediate_(immediate) {}

void
SgAsmAarch64CImmediateOperand::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
