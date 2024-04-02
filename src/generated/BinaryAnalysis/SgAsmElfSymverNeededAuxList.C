//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededAuxList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverNeededAuxList_IMPL
#include <SgAsmElfSymverNeededAuxList.h>

SgAsmElfSymverNeededAuxPtrList const&
SgAsmElfSymverNeededAuxList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverNeededAuxPtrList&
SgAsmElfSymverNeededAuxList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverNeededAuxList::set_entries(SgAsmElfSymverNeededAuxPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverNeededAuxPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymverNeededAuxList::~SgAsmElfSymverNeededAuxList() {
    destructorHelper();
}

SgAsmElfSymverNeededAuxList::SgAsmElfSymverNeededAuxList() {}

void
SgAsmElfSymverNeededAuxList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
