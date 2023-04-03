#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretationList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationPtrList const&
SgAsmInterpretationList::get_interpretations() const {
    return p_interpretations;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationPtrList&
SgAsmInterpretationList::get_interpretations() {
    return p_interpretations;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretationList::set_interpretations(SgAsmInterpretationPtrList const& x) {
    this->p_interpretations = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationList::~SgAsmInterpretationList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretationList::SgAsmInterpretationList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretationList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
