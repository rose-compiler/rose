//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64SysMoveOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64SysMoveOperand_IMPL
#include <sage3basic.h>

unsigned const&
SgAsmAarch64SysMoveOperand::access() const {
    return access_;
}

void
SgAsmAarch64SysMoveOperand::access(unsigned const& x) {
    this->access_ = x;
    set_isModified(true);
}

SgAsmAarch64SysMoveOperand::~SgAsmAarch64SysMoveOperand() {
    destructorHelper();
}

SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand() {}

// The association between constructor arguments and their classes:
//    property=access           class=SgAsmAarch64SysMoveOperand
SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand(unsigned const& access)
    : access_(access) {}

void
SgAsmAarch64SysMoveOperand::initializeProperties() {
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
