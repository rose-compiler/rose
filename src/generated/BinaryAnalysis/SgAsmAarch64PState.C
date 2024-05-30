//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64PState            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64PState_IMPL
#include <SgAsmAarch64PState.h>

Rose::BinaryAnalysis::Aarch64PState const&
SgAsmAarch64PState::pstate() const {
    return pstate_;
}

void
SgAsmAarch64PState::pstate(Rose::BinaryAnalysis::Aarch64PState const& x) {
    this->pstate_ = x;
    set_isModified(true);
}

SgAsmAarch64PState::~SgAsmAarch64PState() {
    destructorHelper();
}

SgAsmAarch64PState::SgAsmAarch64PState() {}

// The association between constructor arguments and their classes:
//    property=pstate           class=SgAsmAarch64PState
SgAsmAarch64PState::SgAsmAarch64PState(Rose::BinaryAnalysis::Aarch64PState const& pstate)
    : pstate_(pstate) {}

void
SgAsmAarch64PState::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
