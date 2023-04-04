#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstructionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionPtrList const&
SgAsmInstructionList::get_instructions() const {
    return p_instructions;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionPtrList&
SgAsmInstructionList::get_instructions() {
    return p_instructions;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstructionList::set_instructions(SgAsmInstructionPtrList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionList::~SgAsmInstructionList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionList::SgAsmInstructionList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstructionList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
