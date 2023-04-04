#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLLList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLPtrList const&
SgAsmGenericDLLList::get_dlls() const {
    return p_dlls;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLPtrList&
SgAsmGenericDLLList::get_dlls() {
    return p_dlls;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLLList::set_dlls(SgAsmGenericDLLPtrList const& x) {
    this->p_dlls = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLList::~SgAsmGenericDLLList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLList::SgAsmGenericDLLList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLLList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
