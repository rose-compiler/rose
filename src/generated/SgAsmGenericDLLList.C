#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLLList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLPtrList const&
SgAsmGenericDLLList::get_dlls() const {
    return p_dlls;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLPtrList&
SgAsmGenericDLLList::get_dlls() {
    return p_dlls;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLLList::set_dlls(SgAsmGenericDLLPtrList const& x) {
    this->p_dlls = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLList::~SgAsmGenericDLLList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLList::SgAsmGenericDLLList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLLList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
