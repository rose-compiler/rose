#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretationList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationPtrList const&
SgAsmInterpretationList::get_interpretations() const {
    return p_interpretations;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationPtrList&
SgAsmInterpretationList::get_interpretations() {
    return p_interpretations;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretationList::set_interpretations(SgAsmInterpretationPtrList const& x) {
    this->p_interpretations = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationList::~SgAsmInterpretationList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationList::SgAsmInterpretationList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretationList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
