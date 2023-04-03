#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Coprocessor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmAarch32Coprocessor::coprocessor() const {
    return coprocessor_;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Coprocessor::coprocessor(int const& x) {
    this->coprocessor_ = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::~SgAsmAarch32Coprocessor() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=coprocessor      class=SgAsmAarch32Coprocessor
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor(int const& coprocessor)
    : coprocessor_(coprocessor) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Coprocessor::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
