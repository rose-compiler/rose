#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64CImmediateOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmAarch64CImmediateOperand::immediate() const {
    return immediate_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64CImmediateOperand::immediate(unsigned const& x) {
    this->immediate_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::~SgAsmAarch64CImmediateOperand() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=immediate        class=SgAsmAarch64CImmediateOperand
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64CImmediateOperand::SgAsmAarch64CImmediateOperand(unsigned const& immediate)
    : immediate_(immediate) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64CImmediateOperand::initializeProperties() {
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
