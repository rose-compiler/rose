//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64PrefetchOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64PrefetchOperand_IMPL
#include <sage3basic.h>

Rose::BinaryAnalysis::Aarch64PrefetchOperation const&
SgAsmAarch64PrefetchOperand::operation() const {
    return operation_;
}

void
SgAsmAarch64PrefetchOperand::operation(Rose::BinaryAnalysis::Aarch64PrefetchOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

SgAsmAarch64PrefetchOperand::~SgAsmAarch64PrefetchOperand() {
    destructorHelper();
}

SgAsmAarch64PrefetchOperand::SgAsmAarch64PrefetchOperand() {}

// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64PrefetchOperand
SgAsmAarch64PrefetchOperand::SgAsmAarch64PrefetchOperand(Rose::BinaryAnalysis::Aarch64PrefetchOperation const& operation)
    : operation_(operation) {}

void
SgAsmAarch64PrefetchOperand::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
