#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64SysMoveOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmAarch64SysMoveOperand::access() const {
    return access_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64SysMoveOperand::access(unsigned const& x) {
    this->access_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::~SgAsmAarch64SysMoveOperand() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=access           class=SgAsmAarch64SysMoveOperand
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand(unsigned const& access)
    : access_(access) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64SysMoveOperand::initializeProperties() {
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
