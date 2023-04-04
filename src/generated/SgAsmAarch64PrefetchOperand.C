#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64PrefetchOperand            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64PrefetchOperation const&
SgAsmAarch64PrefetchOperand::operation() const {
    return operation_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64PrefetchOperand::operation(Rose::BinaryAnalysis::Aarch64PrefetchOperation const& x) {
    this->operation_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64PrefetchOperand::~SgAsmAarch64PrefetchOperand() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64PrefetchOperand::SgAsmAarch64PrefetchOperand() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=operation        class=SgAsmAarch64PrefetchOperand
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64PrefetchOperand::SgAsmAarch64PrefetchOperand(Rose::BinaryAnalysis::Aarch64PrefetchOperation const& operation)
    : operation_(operation) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64PrefetchOperand::initializeProperties() {
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
