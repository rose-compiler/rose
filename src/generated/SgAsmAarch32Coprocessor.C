#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Coprocessor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmAarch32Coprocessor::coprocessor() const {
    return coprocessor_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Coprocessor::coprocessor(int const& x) {
    this->coprocessor_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::~SgAsmAarch32Coprocessor() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=coprocessor      class=SgAsmAarch32Coprocessor
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor(int const& coprocessor)
    : coprocessor_(coprocessor) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Coprocessor::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
