#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInstructionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionPtrList const&
SgAsmInstructionList::get_instructions() const {
    return p_instructions;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionPtrList&
SgAsmInstructionList::get_instructions() {
    return p_instructions;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstructionList::set_instructions(SgAsmInstructionPtrList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionList::~SgAsmInstructionList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionList::SgAsmInstructionList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInstructionList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
