//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstructionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmInstructionList_IMPL
#include <SgAsmInstructionList.h>

SgAsmInstructionPtrList const&
SgAsmInstructionList::get_instructions() const {
    return p_instructions;
}

SgAsmInstructionPtrList&
SgAsmInstructionList::get_instructions() {
    return p_instructions;
}

void
SgAsmInstructionList::set_instructions(SgAsmInstructionPtrList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

SgAsmInstructionList::~SgAsmInstructionList() {
    destructorHelper();
}

SgAsmInstructionList::SgAsmInstructionList() {}

void
SgAsmInstructionList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
