//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLLList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericDLLList_IMPL
#include <sage3basic.h>

SgAsmGenericDLLPtrList const&
SgAsmGenericDLLList::get_dlls() const {
    return p_dlls;
}

SgAsmGenericDLLPtrList&
SgAsmGenericDLLList::get_dlls() {
    return p_dlls;
}

void
SgAsmGenericDLLList::set_dlls(SgAsmGenericDLLPtrList const& x) {
    this->p_dlls = x;
    set_isModified(true);
}

SgAsmGenericDLLList::~SgAsmGenericDLLList() {
    destructorHelper();
}

SgAsmGenericDLLList::SgAsmGenericDLLList() {}

void
SgAsmGenericDLLList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
