//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverEntryList_IMPL
#include <sage3basic.h>

SgAsmElfSymverEntryPtrList const&
SgAsmElfSymverEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverEntryPtrList&
SgAsmElfSymverEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverEntryList::set_entries(SgAsmElfSymverEntryPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymverEntryList::~SgAsmElfSymverEntryList() {
    destructorHelper();
}

SgAsmElfSymverEntryList::SgAsmElfSymverEntryList() {}

void
SgAsmElfSymverEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
