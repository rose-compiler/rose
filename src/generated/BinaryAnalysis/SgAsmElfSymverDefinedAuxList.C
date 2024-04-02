//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedAuxList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverDefinedAuxList_IMPL
#include <sage3basic.h>

SgAsmElfSymverDefinedAuxPtrList const&
SgAsmElfSymverDefinedAuxList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverDefinedAuxPtrList&
SgAsmElfSymverDefinedAuxList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverDefinedAuxList::set_entries(SgAsmElfSymverDefinedAuxPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverDefinedAuxPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymverDefinedAuxList::~SgAsmElfSymverDefinedAuxList() {
    destructorHelper();
}

SgAsmElfSymverDefinedAuxList::SgAsmElfSymverDefinedAuxList() {}

void
SgAsmElfSymverDefinedAuxList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
