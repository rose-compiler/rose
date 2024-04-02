//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretationList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmInterpretationList_IMPL
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
    changeChildPointer(this->p_interpretations, const_cast<SgAsmInterpretationPtrList&>(x));
    set_isModified(true);
}

SgAsmInterpretationList::~SgAsmInterpretationList() {
    destructorHelper();
}

SgAsmInterpretationList::SgAsmInterpretationList() {}

void
SgAsmInterpretationList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
