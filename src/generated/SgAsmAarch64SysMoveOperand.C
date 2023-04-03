#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64SysMoveOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmAarch64SysMoveOperand::access() const {
    return access_;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64SysMoveOperand::access(unsigned const& x) {
    this->access_ = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::~SgAsmAarch64SysMoveOperand() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=access           class=SgAsmAarch64SysMoveOperand
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64SysMoveOperand::SgAsmAarch64SysMoveOperand(unsigned const& access)
    : access_(access) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64SysMoveOperand::initializeProperties() {
}

#endif
#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
