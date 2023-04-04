//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretationList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmInterpretationPtrList const&
SgAsmInterpretationList::get_interpretations() const {
    return p_interpretations;
}

SgAsmInterpretationPtrList&
SgAsmInterpretationList::get_interpretations() {
    return p_interpretations;
}

void
SgAsmInterpretationList::set_interpretations(SgAsmInterpretationPtrList const& x) {
    this->p_interpretations = x;
    set_isModified(true);
}

SgAsmInterpretationList::~SgAsmInterpretationList() {
    destructorHelper();
}

SgAsmInterpretationList::SgAsmInterpretationList() {}

void
SgAsmInterpretationList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
