#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64CImmediateOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmAarch64CImmediateOperand::immediate() const {
    return immediate_;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64CImmediateOperand::immediate(unsigned const& x) {
    this->immediate_ = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::~SgAsmAarch64CImmediateOperand() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=immediate        class=SgAsmAarch64CImmediateOperand
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand(unsigned const& immediate)
    : immediate_(immediate) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64CImmediateOperand::initializeProperties() {
}

#endif
#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
