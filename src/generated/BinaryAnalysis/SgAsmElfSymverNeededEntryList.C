//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverNeededEntryList_IMPL
#include <sage3basic.h>

SgAsmElfSymverNeededEntryPtrList const&
SgAsmElfSymverNeededEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverNeededEntryPtrList&
SgAsmElfSymverNeededEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverNeededEntryList::set_entries(SgAsmElfSymverNeededEntryPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverNeededEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymverNeededEntryList::~SgAsmElfSymverNeededEntryList() {
    destructorHelper();
}

SgAsmElfSymverNeededEntryList::SgAsmElfSymverNeededEntryList() {}

void
SgAsmElfSymverNeededEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
