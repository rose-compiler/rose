#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64AtOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64AtOperation const&
SgAsmAarch64AtOperand::operation() const {
    return operation_;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64AtOperand::operation(Rose::BinaryAnalysis::Aarch64AtOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64AtOperand::~SgAsmAarch64AtOperand() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64AtOperand::SgAsmAarch64AtOperand() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64AtOperand
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64AtOperand::SgAsmAarch64AtOperand(Rose::BinaryAnalysis::Aarch64AtOperation const& operation)
    : operation_(operation) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64AtOperand::initializeProperties() {
}

#endif
#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
